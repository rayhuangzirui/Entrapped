// internal
#include "physics_system.hpp"
#include "world_init.hpp"
#include "state_manager.hpp"
#include <iostream>



// Returns the local bounding coordinates scaled by the current size of the entity
vec2 get_bounding_box(const Motion& motion)
{
	// abs is to avoid negative scale due to the facing direction.
	return { abs(motion.scale.x), abs(motion.scale.y) };
}

// Calculate the axis-aligned bounding box (AABB) for the given motion.
// It ensures that the bounding box is always aligned with the x and y axes, regardless of the rotation.
// Preventing the bb from shrinking when the entity is rotated.
vec2 get_aabb(const Motion& motion)
{
    // Calculate the half-extents of the bounding box
    vec2 half_extents = { abs(motion.scale.x) / 2.0f, abs(motion.scale.y) / 2.0f };
    float cos_angle = abs(cos(motion.angle));
    float sin_angle = abs(sin(motion.angle));

    // Calculate the axis-aligned bounding box (AABB) dimensions
    float aabb_width = half_extents.x * cos_angle + half_extents.y * sin_angle;
    float aabb_height = half_extents.x * sin_angle + half_extents.y * cos_angle;

    // Return the full size of the bounding box
    return { 2.0f * aabb_width, 2.0f * aabb_height };
}

void draw_debug_bounding_boxes() {
    ComponentContainer<Motion>& motion_container = registry.motions;
    for (uint i = 0; i < motion_container.components.size(); i++) {
        Motion& motion = motion_container.components[i];
        Entity entity = motion_container.entities[i];

        if (!registry.players.has(entity) && !registry.enemies.has(entity)) {
            //!registry.eatables.has(entity)
            continue;
        }

        vec2 bb = get_aabb(motion);
        vec2 pos = motion.position;
        //vec2 scale = motion.scale;

        Entity leftLine = createLine({ pos.x - bb.x / 2, pos.y }, { 5.f, bb.y });
        Entity rightLine = createLine({ pos.x + bb.x / 2, pos.y }, { 5.f, bb.y });
        Entity topLine = createLine({ pos.x, pos.y + bb.y / 2 }, { bb.x, 5.f });
        Entity bottomLine = createLine({ pos.x, pos.y - bb.y / 2 }, { bb.x, 5.f });

        // Change the color of the debug bounding box when the entity collides with each other
        vec3 color = { 0.f, 1.f, 0.f }; // green

        if (registry.collisions.has(entity)) {
            color = { 1.f, 0.f, 0.f }; // red
        }

        // print the color
        //std::cout << "Color: " << color.x << " " << color.y << " " << color.z << std::endl;

        registry.colors.emplace(leftLine, color);
        registry.colors.emplace(rightLine, color);
        registry.colors.emplace(topLine, color);
        registry.colors.emplace(bottomLine, color);
    }
    
}


// Check if two aabb intersect
bool aabb_intersect(const vec2& aabb1_pos, const vec2& aabb1_size, const vec2& aabb2_pos, const vec2& aabb2_size) {
    return !(aabb1_pos.x - aabb1_size.x / 2 > aabb2_pos.x + aabb2_size.x / 2 || // aabb1 is at the right of aabb2
        aabb1_pos.x + aabb1_size.x / 2 < aabb2_pos.x - aabb2_size.x / 2 || // aabb1 is at the left of aabb2
        aabb1_pos.y - aabb1_size.y / 2 > aabb2_pos.y + aabb2_size.y / 2 || // aabb1 is above aabb2
        aabb1_pos.y + aabb1_size.y / 2 < aabb2_pos.y - aabb2_size.y / 2); // aabb1 is below aabb2
}

// Check if a point is inside a bounding box, aabb
bool point_in_aabb(const vec2& point, const vec2& aabb_pos, const vec2& aabb_size) {
    return !(point.x < aabb_pos.x - aabb_size.x / 2 || // point is at the left of the aabb
        point.x > aabb_pos.x + aabb_size.x / 2 || // point is at the right of the aabb
        point.y < aabb_pos.y - aabb_size.y / 2 || // point is below the aabb
        point.y > aabb_pos.y + aabb_size.y / 2); // point is above the aabb
}

// Transform a vertex by position, scale, and rotation from local space to world space
vec2 transform_vertex(const vec3& vertex, const Motion& motion) {
    // Scale the vertex
    vec2 scaled_vertex = { vertex.x * motion.scale.x, vertex.y * motion.scale.y };

    // Rotate the vertex
    float s = sin(motion.angle);
    float c = cos(motion.angle);
    vec2 rotated_vertex = { scaled_vertex.x * c - scaled_vertex.y * s, scaled_vertex.x * s + scaled_vertex.y * c };

    // Apply the scale, rotation, and translation to the vertex
    return rotated_vertex + motion.position;
}

// Check if two lines intersect
bool line_intersect(const vec2& p1, const vec2& p2, const vec2& q1, const vec2& q2) {
    // Calculate direction vectors
    vec2 r = p2 - p1;
    vec2 s = q2 - q1;
    float r_cross_s = r.x * s.y - r.y * s.x;
    float t = (q1 - p1).x * s.y - (q1 - p1).y * s.x;
    float u = (q1 - p1).x * r.y - (q1 - p1).y * r.x;

    if (r_cross_s == 0) {
        // Lines are parallel or collinear
        return false;
    }
    t /= r_cross_s;
    u /= r_cross_s;

    return (t >= 0 && t <= 1) && (u >= 0 && u <= 1);
}

// Check if a line intersects a triangle
bool line_triangle_intersect(const vec2& p1, const vec2& p2, const vec2& v0, const vec2& v1, const vec2& v2) {
    // Check for intersection with each edge of the triangle
    return line_intersect(p1, p2, v0, v1) ||
        line_intersect(p1, p2, v1, v2) ||
        line_intersect(p1, p2, v2, v0);
}

// Check if two triangles intersect
bool triangle_intersect(const vec2& t1_v0, const vec2& t1_v1, const vec2& t1_v2,
    const vec2& t2_v0, const vec2& t2_v1, const vec2& t2_v2) {
    return line_triangle_intersect(t1_v0, t1_v1, t2_v0, t2_v1, t2_v2) ||
        line_triangle_intersect(t1_v1, t1_v2, t2_v0, t2_v1, t2_v2) ||
        line_triangle_intersect(t1_v2, t1_v0, t2_v0, t2_v1, t2_v2) ||
        line_triangle_intersect(t2_v0, t2_v1, t1_v0, t1_v1, t1_v2) ||
        line_triangle_intersect(t2_v1, t2_v2, t1_v0, t1_v1, t1_v2) ||
        line_triangle_intersect(t2_v2, t2_v0, t1_v0, t1_v1, t1_v2);
}

// Mesh-mesh collision
void handle_mesh_mesh_collision() {
    auto& motion_registry = registry.motions;
    auto& collidable_registry = registry.collidables;
    auto& enemy_registry = registry.enemies;

    for (uint i = 0; i < collidable_registry.size(); i++) {
        Entity entity_i = collidable_registry.entities[i];
        Motion& motion_i = motion_registry.get(entity_i);
        if (!registry.meshPtrs.has(entity_i)) continue;
        Mesh& mesh_i = *registry.meshPtrs.get(entity_i);

        for (uint j = i + 1; j < collidable_registry.size(); j++) {
            Entity entity_j = collidable_registry.entities[j];
            Motion& motion_j = motion_registry.get(entity_j);
            if (!registry.meshPtrs.has(entity_j)) continue;
            Mesh& mesh_j = *registry.meshPtrs.get(entity_j);

            bool collision_detected = false;
            // don't check collisions between enemies
            if (enemy_registry.has(entity_i) && enemy_registry.has(entity_j)) {
                continue;
            }

			// AABB check before mesh-mesh collision
			vec2 aabb_i = get_aabb(motion_i);
			vec2 aabb_j = get_aabb(motion_j);
            if (!aabb_intersect(motion_i.position, aabb_i, motion_j.position, aabb_j)) {
                continue;
            }

            // Loop through all triangles in mesh_i and mesh_j
            for (size_t k = 0; k < mesh_i.vertex_indices.size(); k += 3) {
                vec2 t1_v0 = transform_vertex(mesh_i.vertices[mesh_i.vertex_indices[k]].position, motion_i);
                vec2 t1_v1 = transform_vertex(mesh_i.vertices[mesh_i.vertex_indices[k + 1]].position, motion_i);
                vec2 t1_v2 = transform_vertex(mesh_i.vertices[mesh_i.vertex_indices[k + 2]].position, motion_i);

                for (size_t l = 0; l < mesh_j.vertex_indices.size(); l += 3) {
                    vec2 t2_v0 = transform_vertex(mesh_j.vertices[mesh_j.vertex_indices[l]].position, motion_j);
                    vec2 t2_v1 = transform_vertex(mesh_j.vertices[mesh_j.vertex_indices[l + 1]].position, motion_j);
                    vec2 t2_v2 = transform_vertex(mesh_j.vertices[mesh_j.vertex_indices[l + 2]].position, motion_j);

                    if (triangle_intersect(t1_v0, t1_v1, t1_v2, t2_v0, t2_v1, t2_v2)) {
                        registry.collisions.emplace_with_duplicates(entity_i, entity_j);
                        registry.collisions.emplace_with_duplicates(entity_j, entity_i);
                        collision_detected = true;
                        break;
                    }
                }
                if (collision_detected) break;
            }
        }
    }
}

// This is a SUPER APPROXIMATE check that puts a circle around the bounding boxes and sees
// if the center point of either object is inside the other's bounding-box-circle. You can
// surely implement a more accurate detection
bool collides(const Motion& motion1, const Motion& motion2)
{
	vec2 dp = motion1.position - motion2.position;
	float dist_squared = dot(dp,dp);
	const vec2 other_bonding_box = get_bounding_box(motion1) / 2.f;
	const float other_r_squared = dot(other_bonding_box, other_bonding_box);
	const vec2 my_bonding_box = get_bounding_box(motion2) / 2.f;
	const float my_r_squared = dot(my_bonding_box, my_bonding_box);
	const float r_squared = max(other_r_squared, my_r_squared);
	if (dist_squared < r_squared)
		return true;
	return false;
}

// check wall collision based on the bounding box
void handle_mesh_box_collision() {
    auto& collidable_registry = registry.collidables;
    auto& motion_registry = registry.motions;
    // Handle mesh-based collision detection
    for (uint i = 0; i < collidable_registry.size(); i++)
    {
        Entity entity_i = collidable_registry.entities[i];
        Motion& motion_i = motion_registry.get(entity_i);

        for (uint j = i + 1; j < collidable_registry.size(); j++) {
            Entity entity_j = collidable_registry.entities[j];
            Motion& motion_j = motion_registry.get(entity_j);

            if (registry.meshPtrs.has(entity_i) && registry.meshPtrs.has(entity_j)) {
                continue;
            }

            if (!registry.meshPtrs.has(entity_i) && !registry.meshPtrs.has(entity_j)) {
                continue;
            }

            vec2 aabb_i = get_aabb(motion_i);
            vec2 aabb_j = get_aabb(motion_j);

            Mesh mesh_sub;
            Motion motion_sub;
            vec2 aabb_sub;
            Motion motion_ob;
            vec2 aabb_ob;
            if (registry.meshPtrs.has(entity_i)) {
                mesh_sub = *registry.meshPtrs.get(entity_i);
                motion_sub = motion_i;
                motion_ob = motion_j;
                aabb_sub = aabb_i;
                aabb_ob = aabb_j;
            }
            else {
                mesh_sub = *registry.meshPtrs.get(entity_j);
                motion_sub = motion_j;
                motion_ob = motion_i;
                aabb_sub = aabb_j;
                aabb_ob = aabb_i;
            }

            // Fast AABB check

            if (!aabb_intersect(motion_i.position, aabb_i, motion_j.position, aabb_j)) {
                continue;
            }

            bool collision = false;

            for (auto& vertex_index : mesh_sub.vertex_indices) {
                vec3 vertex = mesh_sub.vertices[vertex_index].position;
                vec2 transformed_vertex = transform_vertex(vertex, motion_sub);

                // Check if the transformed vertex is inside the player's bounding box
                if (point_in_aabb(transformed_vertex, motion_ob.position, aabb_ob)) {
                    collision = true;
                }

                if (collision) {
                    // Create a collisions event
                    // We are abusing the ECS system a bit in that we potentially insert multiple collisions for the same entity
                    //printf("collision detected\n");
                    registry.collisions.emplace_with_duplicates(entity_i, entity_j);
                    registry.collisions.emplace_with_duplicates(entity_j, entity_i);
                    break;
                }
            }

        }
    }
}

vec2 check_wall_collision(BoundingBox& bb, Motion& motion, int direction) {
    bb.min = motion.position - (abs(motion.scale) / 2.0f);
    bb.max = motion.position + (abs(motion.scale) / 2.0f);
    const int TILE_SIZE = 48;
    vec2 offset = vec2(0, 0);

    int y_top = clamp_m(floor(round_to_digits(bb.min.y / TILE_SIZE, 6)), 0, state.map_height - 1);
    int y_bot = clamp_m(floor(round_to_digits(bb.max.y / TILE_SIZE, 6)), 0, state.map_height - 1);
    int x_left = clamp_m(floor(round_to_digits(bb.min.x / TILE_SIZE, 6)), 0, state.map_width - 1);
    int x_right = clamp_m(floor(round_to_digits(bb.max.x / TILE_SIZE, 6)), 0, state.map_width - 1);

    float offset_top = 0.0;
    float offset_bottom = 0.0;
    float offset_left = 0.0;
    float offset_right = 0.0;

    bool heading_up = motion.velocity.y < 0;
    bool heading_down = motion.velocity.y > 0;
    bool heading_left = motion.velocity.x < 0;
    bool heading_right = motion.velocity.x > 0;

    if (direction == 0) {
        for (uint i = x_left; i < x_right; i++) {
            if (heading_up &&  state.map[y_top][i] == 1) {
                offset_top = max((y_top + 1) * TILE_SIZE - bb.min.y, offset_top);
            }
            if (heading_down && state.map[y_bot][i] == 1) {
                offset_bottom = max(bb.max.y - y_bot * TILE_SIZE, offset_bottom);
            }
        }
    }
    else {
        for (uint i = y_top; i < y_bot; i++) {
            if (heading_left && state.map[i][x_left] == 1) {
                offset_left = max((x_left + 1) * TILE_SIZE - bb.min.x, offset_left);
            }
            if (heading_right && state.map[i][x_right] == 1) {
                offset_right = max(bb.max.x - x_right * TILE_SIZE, offset_right);
            }
        }
    }

    vec2 result = vec2(0, 0);
    //if (offset_top <= 0.001 || offset_bottom <= 0.0010) {
    //    result.y = offset_top - offset_bottom;
    //}
    //if (offset_left <= 0.001 || offset_right <= 0.001) {
    //    result.x = offset_left - offset_right;
    //}
    result.y = offset_top - offset_bottom;
    result.x = offset_left - offset_right;


    //if (offset_top > 0 || offset_bottom > 0 || offset_left > 0 || offset_right > 0) {
    //    printf("bbox: %f, %f, %f, %f\n", bb.min.y, bb.max.y, bb.min.x, bb.max.x);
    //    printf("collision index: %d, %d, %d, %d\n", y_top, y_bot, x_left, x_right);
    //    printf("collision offset: %f, %f, %f, %f\n", offset_top, offset_bottom, offset_left, offset_right);
    //    printf("collision vector: %f, %f\n", result.x, result.y);
    //}

    return result;
}

bool check_box_in_the_wall(Motion motion) {
    const int TILE_SIZE = state.TILE_SIZE;
    vec2 aabb_size = get_aabb(motion);
    vec2 aabb_min = motion.position - aabb_size / 2.0f;
    vec2 aabb_max = motion.position + aabb_size / 2.0f;

    int x_min = int(floor(aabb_min.x / TILE_SIZE));
    int x_max = int(floor(aabb_max.x / TILE_SIZE));
    int y_min = int(floor(aabb_min.y / TILE_SIZE));
    int y_max = int(floor(aabb_max.y / TILE_SIZE));

    // Clamp to map boundaries
    x_min = clamp_m(x_min, 0, state.map_width - 1);
    x_max = clamp_m(x_max, 0, state.map_width - 1);
    y_min = clamp_m(y_min, 0, state.map_height - 1);
    y_max = clamp_m(y_max, 0, state.map_height - 1);

    for (int y = y_min; y <= y_max; ++y) {
        for (int x = x_min; x <= x_max; ++x) {
            if (state.map[y][x] == 1 || state.map[y][x] == 3 || state.map[y][x] == 4) {
                return true;
            }
        }
    }

    return false;
}

// Precise mesh-wall collision handling
void handle_mesh_wall_collision() {
    auto& motion_registry = registry.motions;
    auto& collidable_registry = registry.collidables;
    const int TILE_SIZE = state.TILE_SIZE;

    for (uint i = 0; i < collidable_registry.size(); ++i) {
        Entity entity = collidable_registry.entities[i];
        Motion& motion = motion_registry.get(entity);

        if (!registry.meshPtrs.has(entity)) {
            continue;
        }

        Mesh& mesh = *registry.meshPtrs.get(entity);

        vec2 aabb_size = get_aabb(motion);
        vec2 aabb_min = motion.position - aabb_size / 2.0f;
        vec2 aabb_max = motion.position + aabb_size / 2.0f;

        int x_min = int(floor(aabb_min.x / TILE_SIZE));
        int x_max = int(floor(aabb_max.x / TILE_SIZE));
        int y_min = int(floor(aabb_min.y / TILE_SIZE));
        int y_max = int(floor(aabb_max.y / TILE_SIZE));

        // Clamp to map boundaries
        x_min = clamp_m(x_min, 0, state.map_width - 1);
        x_max = clamp_m(x_max, 0, state.map_width - 1);
        y_min = clamp_m(y_min, 0, state.map_height - 1);
        y_max = clamp_m(y_max, 0, state.map_height - 1);

        bool is_box_in_wall = check_box_in_the_wall(motion);
        if (!is_box_in_wall) {
            continue;
        }
        // For each tile that the entity overlaps with
        for (int y = y_min; y <= y_max; ++y) {
            for (int x = x_min; x <= x_max; ++x) {
                if (state.map[y][x] == 1 || state.map[y][x] == 3 || state.map[y][x] == 4) {
                    vec2 wall_pos = vec2((x + 0.5f) * TILE_SIZE, (y + 0.5f) * TILE_SIZE);

                    vec2 wall_size = vec2(TILE_SIZE, TILE_SIZE);

                    bool collision_detected = false;

                    // For each vertex in the mesh
                    for (size_t vi = 0; vi < mesh.vertex_indices.size(); ++vi) {
                        vec3 vertex = mesh.vertices[mesh.vertex_indices[vi]].position;
                        vec2 transformed_vertex = transform_vertex(vertex, motion);

                        // Check if the vertex is inside the wall's bounding box
                        if (point_in_aabb(transformed_vertex, wall_pos, wall_size)) {
                            collision_detected = true;
                            break;
                        }
                    }
          //          if (registry.players.has(entity)) {

          //              if (collision_detected) {
						    //printf("collision detected\n");
          //                  // print number of tiles
						    //printf("number of tiles: %d, %d\n", x, y);
          //              }
          //              else {
						    //printf("no collision detected\n");
						    //printf("number of tiles: %d, %d\n", x, y);
          //              }
          //          }

                    if (collision_detected) {
						// if the entity is a bullet, remove it
                        if (registry.bullets.has(entity)) {
                            registry.remove_all_components_of(entity);
                        }
                        float overlap_x = 0.0f;
                        float overlap_y = 0.0f;

                        float aabb_left = motion.position.x - aabb_size.x / 2.0f;
                        float aabb_right = motion.position.x + aabb_size.x / 2.0f;
                        float aabb_top = motion.position.y - aabb_size.y / 2.0f;
                        float aabb_bottom = motion.position.y + aabb_size.y / 2.0f;

                        float wall_left = wall_pos.x - wall_size.x / 2.0f;
                        float wall_right = wall_pos.x + wall_size.x / 2.0f;
                        float wall_top = wall_pos.y - wall_size.y / 2.0f;
                        float wall_bottom = wall_pos.y + wall_size.y / 2.0f;

                        // Calculate the minimum translation distance along x
                        if (motion.position.x < wall_pos.x) {
                            overlap_x = aabb_right - wall_left;
                        }
                        else {
                            overlap_x = wall_right - aabb_left;
                        }

                        // Calculate the minimum translation distance along y
                        if (motion.position.y < wall_pos.y) {
                            overlap_y = aabb_bottom - wall_top;
                        }
                        else {
                            overlap_y = wall_bottom - aabb_top;
                        }

                        // Determine the axis of minimum penetration
                        if (overlap_x < overlap_y) {
                            // Move entity out along x-axis
                            float mtv_x = (motion.position.x < wall_pos.x) ? -overlap_x : overlap_x;
                            motion.position.x += mtv_x;

                            // Adjust velocity along x-axis if moving towards the wall
                            if ((mtv_x > 0 && motion.velocity.x < 0) || (mtv_x < 0 && motion.velocity.x > 0)) {
                                motion.velocity.x = 0.0f;
                            }
                        }
                        else {
                            // Move entity out along y-axis
                            float mtv_y = (motion.position.y < wall_pos.y) ? -overlap_y : overlap_y;
                            motion.position.y += mtv_y;

                            // Adjust velocity along y-axis if moving towards the wall
                            if ((mtv_y > 0 && motion.velocity.y < 0) || (mtv_y < 0 && motion.velocity.y > 0)) {
                                motion.velocity.y = 0.0f;
                            }
                        }

  
                    }
                    
                    is_box_in_wall = check_box_in_the_wall(motion);
                }
                if (!is_box_in_wall) {
                    break;
                }
            }

            if (!is_box_in_wall) {
                break;
            }
        }
    }
}

//void handle_bullet_wall_collision(float step_seconds) {
//    auto& motion_registry = registry.motions;
//    auto& bbox_container = registry.boundingBoxes;
//
//    // Iterate over all bullet entities
//    for (uint i = 0; i < registry.bullets.entities.size(); i++) {
//        Entity bullet = registry.bullets.entities[i];
//        Motion& bullet_motion = motion_registry.get(bullet);
//
//        // Move the bullet based on its current velocity
//        bullet_motion.position += bullet_motion.velocity * step_seconds;
//
//        // Get the bullet's AABB
//        vec2 bullet_aabb = get_aabb(bullet_motion);
//
//        // Flag to track collision
//        bool collision_occurred = false;
//
//        // Check for collision with wall entities
//        for (uint j = 0; j < bbox_container.components.size(); j++) {
//            Entity wall_entity = bbox_container.entities[j];
//            BoundingBox& wall_bbox = bbox_container.get(wall_entity);
//
//            // Skip non-wall entities by checking if they have the wall component (if applicable)
//            if (registry.bullets.has(wall_entity)) continue;
//
//            // Get the wall's AABB
//            vec2 wall_aabb = get_aabb(motion_registry.get(wall_entity));
//
//            // Check if the bullet intersects with the wall
//            if (aabb_intersect(bullet_motion.position, bullet_aabb, motion_registry.get(wall_entity).position, wall_aabb)) {
//                // Collision detected - stop the bullet by setting its velocity to zero
//                bullet_motion.velocity = { 0.0f, 0.0f };
//                collision_occurred = true;
//                break;
//            }
//        }
//    }
//}

const float DASH_MULTIPLIER = 5.0f;


void PhysicsSystem::step(float elapsed_ms) 
{
    //return;
    // Move fish based on how much time has passed, this is to (partially) avoid
    // having entities move at different speed based on the machine.
    auto& motion_registry = registry.motions;
    auto& collidable_registry = registry.collidables;
    auto& bbox_container = registry.boundingBoxes;
    float step_seconds = elapsed_ms / 1000.f;

    /*handle_bullet_wall_collision(step_seconds);*/

	// Continuous collision detection: sub-step the motion to avoid tunneling
    const float max_substep_distance = 5.0f; 

    for (uint i = 0; i < motion_registry.components.size(); i++) {
        Motion& motion = motion_registry.components[i];
        Entity& entity = motion_registry.entities[i];

        float velocity_multiplier = 1.0f;
        if (registry.dashTimers.has(entity)) {
            velocity_multiplier = DASH_MULTIPLIER;
            DashTimer& dash = registry.dashTimers.get(entity);
            
            // Update dash timer
            dash.counter_ms -= elapsed_ms;
            dash.cooldown_ms -= elapsed_ms;
            if (dash.counter_ms <= 0) {
				// Reset velocity multiplier
                velocity_multiplier = 1.0f;
            }

            if (dash.cooldown_ms <= 0) {
                registry.dashTimers.remove(entity);
            }
        }
        vec2 total_movement = motion.velocity * velocity_multiplier * step_seconds;
        float distance = length(total_movement);
        int sub_steps = std::max(1, int(distance / max_substep_distance));

        // Calculate movement per sub-step
        vec2 movement_per_substep = total_movement / float(sub_steps);
        float substep_seconds = step_seconds / float(sub_steps);

		// Continuous collision detection for wall collisions over multiple sub-steps
        for (int step = 0; step < sub_steps; ++step) {
            motion.position += movement_per_substep;
        }
    }

    handle_mesh_wall_collision();
    // Mesh-wall collision
    //if (registry.meshPtrs.has(entity)) {
    //    Mesh& mesh = *registry.meshPtrs.get(entity);
    //    BoundingBox& bb = bbox_container.get(entity);

    //    //handle_wall_mesh_collision(entity, bb, motion, mesh);
    //    handle_wall_mesh_collision(entity, bb, motion, mesh);
    //}

    // Currently collisions are mesh-based: player-enemy, bullet-enemy
    handle_mesh_mesh_collision();

    // Mesh-box collision
    //handle_mesh_box_collision();
    handle_mesh_box_collision();


    // Update gun position
    if (registry.players.size() > 0) {
        Entity player_entity = registry.players.entities[0];
        Motion& player_motion = motion_registry.get(player_entity);
        if (registry.guns.entities.size() > 0) {
            // update gun position
            Entity gun = registry.guns.entities[0];
            Gun& gun_component = registry.guns.get(gun);
            Motion& gun_motion = motion_registry.get(gun);
            gun_motion.position = player_motion.position + gun_component.offset;
        }
    }

    if (debugging.in_debug_mode) {
        draw_debug_bounding_boxes();
    }
}
