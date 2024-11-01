// internal
#include "physics_system.hpp"
#include "world_init.hpp"
#include "maze.hpp"
#include <iostream>


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

int clamp_m(int n, int min_v, int max_v) {
    return min(max(n, min_v), max_v);
}

//vec4 check_wall_collision(const BoundingBox& bb) {
//    const int TILE_SIZE = 48;
//    int y_top = floor(bb.min.y/TILE_SIZE);
//    int y_bot = floor(bb.max.y / TILE_SIZE);
//    int x_left = floor(bb.min.x / TILE_SIZE);
//    int x_right = floor(bb.max.x / TILE_SIZE);
//    //printf("collision index: %d, %d, %d, %d\n", y_top, y_bot, x_left, x_right);
//
//    vec4 result = { 0, 0, 0, 0 };
//
//    // check in box
//    for (uint i = x_left+1; i < x_right; i++) {
//        if (box_testing_environment[y_top][i] == 1) {
//            result.x = (y_top + 1) * TILE_SIZE - bb.min.y;
//        }
//        if (box_testing_environment[y_bot][i] == 1) {
//            result.y = bb.max.y - y_bot * TILE_SIZE;
//        }
//    }
//
//    for (uint i = y_top; i < y_bot; i++) {
//        if (box_testing_environment[i][x_left] == 1) {
//            result.z = (x_left + 1) * TILE_SIZE - bb.min.x;
//        }
//        if (box_testing_environment[i][x_right] == 1) {
//            result.w = bb.max.x - x_right * TILE_SIZE;
//        }
//    }
//    if (result.x > 0 || result.y > 0 || result.z > 0 || result.w > 0) {
//        //printf("collision vector: %f, %f, %f, %f\n", result.x, result.y, result.z, result.w);
//    }
//
//    return result;
//}

// borrowed from https://stackoverflow.com/questions/13094224/a-c-routine-to-round-a-float-to-n-significant-digits
float round_to_digits(float value, int digits)
{
    if (value == 0.0) // otherwise it will return 'nan' due to the log10() of zero
        return 0.0;

    float factor = pow(10.0, digits - ceil(log10(fabs(value))));
    return round(value * factor) / factor;
}

//vec2 check_wall_collision_new(Motion& motion, vec2 initial_position,  int direction) {
//    vec2 bb_min = motion.position - (abs(motion.scale) / 2.0f);
//    vec2 bb_max = motion.position + (abs(motion.scale) / 2.0f);
//    const int TILE_SIZE = 48;
//    vec2 offset = vec2(0, 0);
//
//    int y_top = clamp_m(floor(round_to_digits(bb_min.y / TILE_SIZE, 6)), 0, BOX_MAZE_HEIGHT-1);
//    int y_bot = floor(round_to_digits(bb_max.y / TILE_SIZE, 6));
//    int x_left = floor(round_to_digits(bb_min.x / TILE_SIZE, 6));
//    int x_right = floor(round_to_digits(bb_max.x / TILE_SIZE, 6));
//
//    vec2 bb_min_init = initial_position - (abs(motion.scale) / 2.0f);
//    vec2 bb_max_init = initial_position + (abs(motion.scale) / 2.0f);
//
//    int y_top_init = floor(round_to_digits(bb_min.y / TILE_SIZE, 6));
//    int y_bot_init = floor(round_to_digits(bb_max.y / TILE_SIZE, 6));
//    int x_left_init = floor(round_to_digits(bb_min.x / TILE_SIZE, 6));
//    int x_right_init = floor(round_to_digits(bb_max.x / TILE_SIZE, 6));
//
//    float offset_top = 0.0;
//    float offset_bottom = 0.0;
//    float offset_left = 0.0;
//    float offset_right = 0.0;
//
//    bool heading_up = motion.velocity.y < 0;
//    bool heading_down = motion.velocity.y > 0;
//    bool heading_left = motion.velocity.x < 0;
//    bool heading_right = motion.velocity.x > 0;
//
//
//    if (direction == 0) {
//        for (uint i = x_left; i < x_right; i++) {
//            if (heading_up && (y_top < 0 || box_testing_environment[y_top][i] == 1)) {
//                offset_top = max((y_top + 1) * TILE_SIZE - bb.min.y, offset_top);
//            }
//            if (heading_down && (y_bot >= BOX_MAZE_HEIGHT || box_testing_environment[y_bot][i] == 1)) {
//                offset_bottom = max(bb.max.y - y_bot * TILE_SIZE, offset_bottom);
//            }
//        }
//    }
//    else {
//        for (uint i = y_top; i < y_bot; i++) {
//            if (heading_left && (x_left < 0 || box_testing_environment[i][x_left] == 1)) {
//                offset_left = max((x_left + 1) * TILE_SIZE - bb.min.x, offset_left);
//            }
//            if (heading_right && (x_right >= BOX_MAZE_WIDTH || box_testing_environment[i][x_right] == 1)) {
//                offset_right = max(bb.max.x - x_right * TILE_SIZE, offset_right);
//            }
//        }
//    }
//
//    vec2 result = vec2(0, 0);
//    //if (offset_top <= 0.001 || offset_bottom <= 0.0010) {
//    //    result.y = offset_top - offset_bottom;
//    //}
//    //if (offset_left <= 0.001 || offset_right <= 0.001) {
//    //    result.x = offset_left - offset_right;
//    //}
//    result.y = offset_top - offset_bottom;
//    result.x = offset_left - offset_right;
//
//
//    //if (offset_top > 0 || offset_bottom > 0 || offset_left > 0 || offset_right > 0) {
//    //    printf("bbox: %f, %f, %f, %f\n", bb.min.y, bb.max.y, bb.min.x, bb.max.x);
//    //    printf("collision index: %d, %d, %d, %d\n", y_top, y_bot, x_left, x_right);
//    //    printf("collision offset: %f, %f, %f, %f\n", offset_top, offset_bottom, offset_left, offset_right);
//    //    printf("collision vector: %f, %f\n", result.x, result.y);
//    //}
//
//    return result;
//}

vec2 check_wall_collision(BoundingBox& bb, Motion& motion, int direction) {
    bb.min = motion.position - (abs(motion.scale) / 2.0f);
    bb.max = motion.position + (abs(motion.scale) / 2.0f);
    const int TILE_SIZE = 48;
    vec2 offset = vec2(0, 0);

    int y_top = clamp_m(floor(round_to_digits(bb.min.y / TILE_SIZE, 6)), 0, BOX_MAZE_HEIGHT - 1);
    int y_bot = clamp_m(floor(round_to_digits(bb.max.y / TILE_SIZE, 6)), 0, BOX_MAZE_HEIGHT - 1);
    int x_left = clamp_m(floor(round_to_digits(bb.min.x / TILE_SIZE, 6)), 0, BOX_MAZE_WIDTH - 1);
    int x_right = clamp_m(floor(round_to_digits(bb.max.x / TILE_SIZE, 6)), 0, BOX_MAZE_WIDTH - 1);

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
            if (heading_up && (y_top < 0 || box_testing_environment[y_top][i] == 1)) {
                offset_top = max((y_top + 1) * TILE_SIZE - bb.min.y, offset_top);
            }
            if (heading_down && (y_bot >= BOX_MAZE_HEIGHT || box_testing_environment[y_bot][i] == 1)) {
                offset_bottom = max(bb.max.y - y_bot * TILE_SIZE, offset_bottom);
            }
        }
    }
    else {
        for (uint i = y_top; i < y_bot; i++) {
            if (heading_left && (x_left < 0 || box_testing_environment[i][x_left] == 1)) {
                offset_left = max((x_left + 1) * TILE_SIZE - bb.min.x, offset_left);
            }
            if (heading_right && (x_right >= BOX_MAZE_WIDTH || box_testing_environment[i][x_right] == 1)) {
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

        if (bbox_container.has(entity)) {
            BoundingBox& bb = bbox_container.get(entity);
            // try y-axis first
            Motion temp = motion;
            vec2 offset_y = check_wall_collision(bb, temp, 0);
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