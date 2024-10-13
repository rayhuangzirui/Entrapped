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
    for (uint i = x_left+1; i < x_right; i++) {
        if (maze[y_top][i] == 1) {
            result.x = (y_top+1) * TILE_SIZE - bb.min.y;
        }
        if (maze[y_bot][i] == 1) {
            result.y = bb.max.y - y_bot * TILE_SIZE;
        }
    }

    for (uint i = y_top+1; i < y_bot; i++) {
        if (maze[i][x_left] == 1) {
            result.z = (x_left+1) * TILE_SIZE - bb.min.x;
        }
        if (maze[i][x_right] == 1) {
            result.w = bb.max.x - x_right * TILE_SIZE;
        }
    }

    // check in box
    for (uint i = x_left; i < x_right; i++) {
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
            vec4 v1 = check_wall_collision(bb);
            //vec2 final_velocity = { motion.velocity.x, motion.velocity.y }
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
            //motion.position = motion.position + vec2(v1.z - v1.w, v1.x - v1.y);
        }
    }

    // Check for collisions between all moving entities
    ComponentContainer<Motion>& motion_container = registry.motions;
    for (uint i = 0; i < motion_container.components.size(); i++) {
        Motion& motion_i = motion_container.components[i];
        Entity entity_i = motion_container.entities[i];

        // Note starting j at i+1 to compare all (i, j) pairs only once (and to not compare with itself)
        for (uint j = i + 1; j < motion_container.components.size(); j++) {
            Motion& motion_j = motion_container.components[j];
            if (collides(motion_i, motion_j)) {
                Entity entity_j = motion_container.entities[j];

                // Create a collisions event
                // We are abusing the ECS system a bit in that we potentially insert multiple collisions for the same entity
                registry.collisions.emplace_with_duplicates(entity_i, entity_j);
                registry.collisions.emplace_with_duplicates(entity_j, entity_i);
            }
        }
    }
}