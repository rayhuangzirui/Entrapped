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

    for (uint i = 0; i < motion_registry.size(); i++) {
        Motion& motion_i = motion_registry.components[i];
        Entity entity_i = motion_registry.entities[i];
        if (!registry.meshPtrs.has(entity_i)) continue;
        Mesh& mesh_i = *registry.meshPtrs.get(entity_i);

        for (uint j = i + 1; j < motion_registry.size(); j++) {
            Motion& motion_j = motion_registry.components[j];
            Entity entity_j = motion_registry.entities[j];
            if (!registry.meshPtrs.has(entity_j)) continue;
            Mesh& mesh_j = *registry.meshPtrs.get(entity_j);

            bool collision_detected = false;

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

int clamp_m(int n, int min_v, int max_v) {
    return min(max(n, min_v), max_v);
}

// borrowed from https://stackoverflow.com/questions/13094224/a-c-routine-to-round-a-float-to-n-significant-digits
float round_to_digits(float value, int digits)
{
    if (value == 0.0) // otherwise it will return 'nan' due to the log10() of zero
        return 0.0;

    float factor = pow(10.0, digits - ceil(log10(fabs(value))));
    return round(value * factor) / factor;
}

// Mesh-mesh collision
void handle_mesh_box_collision() {
    auto& motion_registry = registry.motions;
    // Handle mesh-based collision detection
    for (uint i = 0; i < motion_registry.size(); i++)
    {
        Motion& motion_i = motion_registry.components[i];
        Entity entity_i = motion_registry.entities[i];

        // Skip entities without a mesh
        if (!registry.meshPtrs.has(entity_i)) {
            continue;
        }

        Mesh& mesh_i = *registry.meshPtrs.get(entity_i);

        for (uint j = i + 1; j < motion_registry.size(); j++) {
            Motion& motion_j = motion_registry.components[j];
            Entity entity_j = motion_registry.entities[j];


            if (!registry.meshPtrs.has(entity_j)) {
                continue;
            }

            Mesh& mesh_j = *registry.meshPtrs.get(entity_j);

            vec2 aabb_i = get_aabb(motion_i);
            vec2 aabb_j = get_aabb(motion_j);

            // Fast AABB check
            if (!aabb_intersect(motion_i.position, aabb_i, motion_j.position, aabb_j)) {
                continue;
            }

            bool collision = false;

            for (auto& vertex_index : mesh_i.vertex_indices) {
                vec3 vertex = mesh_i.vertices[vertex_index].position;
                vec2 transformed_vertex = transform_vertex(vertex, motion_i);

                for (auto& vertex_index_j : mesh_j.vertex_indices) {
                    vec3 vertex_j = mesh_j.vertices[vertex_index_j].position;
                    vec2 transformed_player_vertex = transform_vertex(vertex_j, motion_j);

                    // Check if the transformed vertex is inside the player's bounding box
                    if (point_in_aabb(transformed_vertex, motion_j.position, aabb_j)) {
                        collision = true;
                        break;
                    }

                    // Check if the transformed player vertex is inside the entity's bounding box
                    if (point_in_aabb(transformed_player_vertex, motion_i.position, aabb_i)) {
                        collision = true;
                        break;
                    }
                }

                if (collision) {
                    // Create a collisions event
                    // We are abusing the ECS system a bit in that we potentially insert multiple collisions for the same entity
                    printf("collision detected\n");
                    registry.collisions.emplace_with_duplicates(entity_i, entity_j);
                    registry.collisions.emplace_with_duplicates(entity_j, entity_i);
                    break;
                }
            }

        }
    }
}

// check wall collision based on the bounding box
void handle_mesh_box_collision_new() {
    auto& motion_registry = registry.motions;
    // Handle mesh-based collision detection
    for (uint i = 0; i < motion_registry.size(); i++)
    {
        Motion& motion_i = motion_registry.components[i];
        Entity entity_i = motion_registry.entities[i];

        for (uint j = i + 1; j < motion_registry.size(); j++) {
            Motion& motion_j = motion_registry.components[j];
            Entity entity_j = motion_registry.entities[j];

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

void handle_wall_mesh_collision(BoundingBox& bb, Motion& motion, Mesh& mesh) {
    const int TILE_SIZE = 48;
    bb.min = motion.position - (abs(motion.scale) / 2.0f);
    bb.max = motion.position + (abs(motion.scale) / 2.0f);

    int y_top = clamp_m(floor(round_to_digits(bb.min.y / TILE_SIZE, 6)), 0, BOX_MAZE_HEIGHT - 1);
    int y_bot = clamp_m(floor(round_to_digits(bb.max.y / TILE_SIZE, 6)), 0, BOX_MAZE_HEIGHT - 1);
    int x_left = clamp_m(floor(round_to_digits(bb.min.x / TILE_SIZE, 6)), 0, BOX_MAZE_WIDTH - 1);
    int x_right = clamp_m(floor(round_to_digits(bb.max.x / TILE_SIZE, 6)), 0, BOX_MAZE_WIDTH - 1);

	//printf("y_top: %d, y_bot: %d, x_left: %d, x_right: %d\n", y_top, y_bot, x_left, x_right);
    for (uint j = y_top; j <= y_bot; ++j) {
        for (uint i = x_left; i <= x_right; ++i) {
            if (box_testing_environment[j][i] == 1) {
				vec2 wall_pos = vec2((i + 0.5) * TILE_SIZE, (j + 0.5) * TILE_SIZE);
                vec2 wall_size = vec2(TILE_SIZE, TILE_SIZE);
                /*printf("y: %d, x: %d\n", j, i);
                printf("wall_pos: %f, %f\n", wall_pos.x, wall_pos.y);
                printf("player_pos: %f, %f\n", motion.position.x, motion.position.y);*/

				// Quick aabb check
                if (!aabb_intersect(motion.position, get_aabb(motion), wall_pos, wall_size)) {

                    continue;
                }

				// loop through vertices of the mesh
				for (size_t k = 0; k < mesh.vertex_indices.size(); k++) {
					vec2 v0 = transform_vertex(mesh.vertices[mesh.vertex_indices[k]].position, motion);
					if (point_in_aabb(v0, wall_pos, wall_size)) {
						//printf("collision detected\n");
						float wall_left = wall_pos.x - TILE_SIZE / 2;
						float wall_right = wall_pos.x + TILE_SIZE / 2;
						float wall_top = wall_pos.y + TILE_SIZE / 2;
						float wall_bot = wall_pos.y - TILE_SIZE / 2;

                        float x_overlap = 0;
						float y_overlap = 0;

						// X-axis overlap
						if (v0.x < wall_right && v0.x > wall_left) {
							if (v0.x < motion.position.x) {
								x_overlap = wall_right - v0.x;
							}
							else {
								x_overlap = wall_left - v0.x;
							}
						}

						// Y-axis overlap
						if (v0.y < wall_top && v0.y > wall_bot) {
							if (v0.y < motion.position.y) {
								y_overlap = wall_top - v0.y;
							}
							else {
								y_overlap = wall_bot - v0.y;
							}
						}

						// Resolve collision based on the minimum overlap direction (x or y)
						if (abs(x_overlap) < abs(y_overlap)) {
							motion.position.x += x_overlap;
						}
						else {
							motion.position.y += y_overlap;
						}




                        //  if (v0.x < wall_right && v0.x > wall_left+TILE_SIZE/2) {

						//	motion.position.x += wall_right - v0.x;
						//	//printf("new player_pos: %f, %f\n", motion.position.x, motion.position.y);
						//}
						//else if (v0.x > wall_left && v0.x < wall_right - TILE_SIZE / 2) {
						//	motion.position.x -= v0.x - wall_left;
						//}
						//
						//else if (v0.y < wall_top && v0.y > wall_bot + TILE_SIZE / 2) {
						//	motion.position.y += wall_top - v0.y;
						//}
						//else if (v0.y > wall_bot && v0.y < wall_top - TILE_SIZE / 2) {
						//	motion.position.y -= v0.y - wall_bot;
						//}
						
					}
				}
            }
        }
    }
	
}



const float DASH_MULTIPLIER = 5.0f;

void PhysicsSystem::step(float elapsed_ms)
{
    // Move fish based on how much time has passed, this is to (partially) avoid
    // having entities move at different speed based on the machine.
    auto& motion_registry = registry.motions;
    auto& bbox_container = registry.boundingBoxes;
    float step_seconds = elapsed_ms / 1000.f;

    for (uint i = 0; i < motion_registry.components.size(); i++) {
        Motion& motion = motion_registry.components[i];
        Entity& entity = motion_registry.entities[i];

        // Handle dash effect if a DashTimer exists
        if (registry.dashTimers.has(entity)) {
            DashTimer& dash = registry.dashTimers.get(entity);
            motion.position += (motion.velocity * DASH_MULTIPLIER) * step_seconds;

            // Update dash timer
            dash.counter_ms -= elapsed_ms;
            if (dash.counter_ms <= 0) {
                registry.dashTimers.remove(motion_registry.entities[i]);
            }
        }
        else {
            motion.position += motion.velocity * step_seconds;
        }

		// Mesh-wall collision
        if (registry.meshPtrs.has(entity)) {
			Mesh& mesh = *registry.meshPtrs.get(entity);
			BoundingBox& bb = bbox_container.get(entity);

			handle_wall_mesh_collision(bb, motion, mesh);
        }

		// Handle wall collision
        /*if (bbox_container.has(entity)) {
            BoundingBox& bb = bbox_container.get(entity);
            // try y-axis first
            Motion temp = motion;
            vec2 offset_y = check_wall_collision(bb, temp,  0);
            temp.position += offset_y;
            vec2 offset_x = check_wall_collision(bb, temp, 1);
            vec2 offset_y_first = offset_x + offset_y;
            // then x-axis first
            temp = motion;
            offset_x = check_wall_collision(bb, temp, 1);
            temp.position += offset_x;
            offset_y = check_wall_collision(bb, temp, 0);
            vec2 offset_x_first = offset_x + offset_y;

            if (length(offset_y_first) < length(offset_x_first)) {
                motion.position += offset_y_first;
            }
            else {
                motion.position += offset_x_first;
            }

            bb.min = motion.position - (abs(motion.scale) / 2.0f);
            bb.max = motion.position + (abs(motion.scale) / 2.0f);
        }*/
    }

	// Currently collisions are mesh-based: player-enemy, bullet-enemy
	handle_mesh_mesh_collision();

	// Mesh-box collision
	//handle_mesh_box_collision();
    handle_mesh_box_collision_new();


	// Box-box collision below
    
 
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