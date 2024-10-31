// internal
#include "physics_system.hpp"
#include "world_init.hpp"
#include "maze.hpp"

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

        if (!registry.players.has(entity) && !registry.enemies.has(entity) && !registry.eatables.has(entity)) {
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


//bool PhysicsSystem::check_player_wall_collision(const Motion& player_motion) {
//    // Get the player's position in the grid
//    int tile_x = static_cast<int>(player_motion.position.x) / TILE_SIZE;
//    int tile_y = static_cast<int>(player_motion.position.y) / TILE_SIZE;
//
//    // Make sure we're within bounds of the maze
//    if (tile_x >= 0 && tile_x < MAZE_WIDTH && tile_y >= 0 && tile_y < MAZE_HEIGHT) {
//        // Check if the player is trying to move into a wall
//        if (box_testing_environment[tile_y][tile_x] == 1) {
//            printf("wall collision detected");
//            return true; // Collision detected with wall
//        }
//    }
//    return false;
//}

vec4 check_wall_collision(const BoundingBox& bb) {
    const int TILE_SIZE = 48;
    int y_top = floor(bb.min.y/TILE_SIZE);
    int y_bot = floor(bb.max.y / TILE_SIZE);
    int x_left = floor(bb.min.x / TILE_SIZE);
    int x_right = floor(bb.max.x / TILE_SIZE);
    //printf("collision index: %d, %d, %d, %d\n", y_top, y_bot, x_left, x_right);

    vec4 result = { 0, 0, 0, 0 };
    // check in maze
    //for (uint i = x_left+1; i < x_right; i++) {
    //    if (maze[y_top][i] == 1) {
    //        result.x = (y_top+1) * TILE_SIZE - bb.min.y;
    //    }
    //    if (maze[y_bot][i] == 1) {
    //        result.y = bb.max.y - y_bot * TILE_SIZE;
    //    }
    //}

    //for (uint i = y_top+1; i < y_bot; i++) {
    //    if (maze[i][x_left] == 1) {
    //        result.z = (x_left+1) * TILE_SIZE - bb.min.x;
    //    }
    //    if (maze[i][x_right] == 1) {
    //        result.w = bb.max.x - x_right * TILE_SIZE;
    //    }
    //}

    // check in box
    for (uint i = x_left+1; i < x_right; i++) {
        if (box_testing_environment[y_top][i] == 1) {
            result.x = (y_top + 1) * TILE_SIZE - bb.min.y;
        }
        if (box_testing_environment[y_bot][i] == 1) {
            result.y = bb.max.y - y_bot * TILE_SIZE;
        }
    }

    for (uint i = y_top; i < y_bot; i++) {
        if (box_testing_environment[i][x_left] == 1) {
            result.z = (x_left + 1) * TILE_SIZE - bb.min.x;
        }
        if (box_testing_environment[i][x_right] == 1) {
            result.w = bb.max.x - x_right * TILE_SIZE;
        }
    }
    if (result.x > 0 || result.y > 0 || result.z > 0 || result.w > 0) {
        //printf("collision vector: %f, %f, %f, %f\n", result.x, result.y, result.z, result.w);
    }
    return result;
}

const float DASH_MULTIPLIER = 5.0f;

void PhysicsSystem::step(float elapsed_ms)
{
    // Move fish based on how much time has passed, this is to (partially) avoid
    // having entities move at different speed based on the machine.
    auto& motion_registry = registry.motions;
    float step_seconds = elapsed_ms / 1000.f;

    for (uint i = 0; i < motion_registry.components.size(); i++) {
        Motion& motion = motion_registry.components[i];

        // Handle dash effect if a DashTimer exists
        if (registry.dashTimers.has(motion_registry.entities[i])) {
            DashTimer& dash = registry.dashTimers.get(motion_registry.entities[i]);
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

        if (registry.boundingBoxes.has(motion_registry.entities[i])) {
            BoundingBox& bb = registry.boundingBoxes.get(motion_registry.entities[i]);

            bb.min = motion.position - (abs(motion.scale) / 2.0f);
            bb.max = motion.position + (abs(motion.scale) / 2.0f);

            // position correction
            vec4 v1 = check_wall_collision(bb);
            //vec2 final_velocity = { motion.velocity.x, motion.velocity.y }
            int max_attempts = 3;
            int i = 0;

            while (v1.x != 0 || v1.y != 0 || v1.z != 0 || v1.w != 0) {
                if (v1.x != 0) {
                    if (motion.velocity.y < 0) {
                        motion.velocity.y = 0;
                    }
                }
                if (v1.y != 0) {
                    if (motion.velocity.y > 0) {
                        motion.velocity.y = 0;
                    }
                }
                if (v1.z != 0) {
                    if (motion.velocity.x < 0) {
                        motion.velocity.x = 0;
                    }
                }
                if (v1.w != 0) {
                    if (motion.velocity.x > 0) {
                        motion.velocity.x = 0;
                    }
                }
                if (v1.x == 0 || v1.y == 0) {
                    motion.position.y = motion.position.y + (v1.x - v1.y);
                }

                if (v1.z == 0 || v1.w == 0) {
                    motion.position.x = motion.position.x + (v1.z - v1.w);
                }

                bb.min = motion.position - (abs(motion.scale) / 2.0f);
                bb.max = motion.position + (abs(motion.scale) / 2.0f);
                v1 = check_wall_collision(bb);
                i++;
                if (i >= max_attempts) {
                    break;
                }
            }
            //motion.position = motion.position + vec2(v1.z - v1.w, v1.x - v1.y);
        }
    }

	// Currently collisions are mesh-based: player-enemy, bullet-enemy
	handle_mesh_mesh_collision();

	// Mesh-box collision
	//handle_mesh_box_collision();


	// Box-box collision below
    
    // Check for collisions between all moving entities
    //ComponentContainer<Motion>& motion_container = registry.motions;
    //for (uint i = 0; i < motion_container.components.size(); i++) {
    //    Motion& motion_i = motion_container.components[i];
    //    Entity entity_i = motion_container.entities[i];

    //    // Note starting j at i+1 to compare all (i, j) pairs only once (and to not compare with itself)
    //    for (uint j = i + 1; j < motion_container.components.size(); j++) {
    //        Motion& motion_j = motion_container.components[j];
    //        if (collides(motion_i, motion_j)) {
    //            Entity entity_j = motion_container.entities[j];

    //            // Create a collisions event
    //            // We are abusing the ECS system a bit in that we potentially insert multiple collisions for the same entity
    //            registry.collisions.emplace_with_duplicates(entity_i, entity_j);
    //            registry.collisions.emplace_with_duplicates(entity_j, entity_i);
    //        }
    //    }
    //}

    if (debugging.in_debug_mode) {
	    draw_debug_bounding_boxes();
    }

}