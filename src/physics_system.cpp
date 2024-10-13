// internal
#include "physics_system.hpp"
#include "world_init.hpp"
#include "maze.hpp"


// Helper function to calculate bounding box of an entity
BoundingBox calculate_bounding_box(const Motion& motion) {
    vec2 min = motion.position - (motion.scale / 2.0f);
    vec2 max = motion.position + (motion.scale / 2.0f);
    return BoundingBox{ min, max };
}

// Function to check if two bounding boxes overlap (AABB collision detection)
bool check_aabb_collision(const BoundingBox& box1, const BoundingBox& box2) {
    return (box1.min.x < box2.max.x &&
        box1.max.x > box2.min.x &&
        box1.min.y < box2.max.y &&
        box1.max.y > box2.min.y);
}

// Function to check collision between player and walls (tiles)
bool check_player_tile_collision(const Motion& player_motion, const std::vector<BoundingBox>& tile_bounding_boxes) {
    BoundingBox player_box = calculate_bounding_box(player_motion);
    for (const BoundingBox& tile_box : tile_bounding_boxes) {
        if (check_aabb_collision(player_box, tile_box)) {
            return true;  // Collision detected
        }
    }
    return false;  // No collision detected
}

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