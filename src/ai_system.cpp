// internal
#include "ai_system.hpp"
#include <iostream>


void AISystem::step(float elapsed_ms_since_last_update)
{

	auto& enemyAI_container = registry.enemyAIs;
	auto& motion_container = registry.motions;
	auto& player_container = registry.players;
	if (player_container.size() <= 0) {
		return;
	}
	Entity player = player_container.entities[0];
	for (uint i = 0; i < enemyAI_container.components.size(); i++) {
		EnemyAI& enemyAI = enemyAI_container.components[i];
		Entity& entity = enemyAI_container.entities[i];
		Motion& motion = motion_container.get(entity);
		Motion& player_motion = motion_container.get(player);
		Health& health = registry.healths.get(entity);

		// states - root of the decision tree
		// wandering state: 0
		// chasing state: 1
		// fleeing state: 2
		if (enemyAI.state == 0) {
			// if the player is within a certain distance, switch to chasing
			if (distance(player_motion.position, motion.position) <= enemyAI.detection_radius) {
				enemyAI.state = 1;
				enemyAI.wander_timer = 0;
				enemyAI.last_player_position = player_motion.position;
				continue;
			}
			// else, pick a random direction every 2 seconds
			else {
				enemyAI.wander_timer -= elapsed_ms_since_last_update;
				if (enemyAI.wander_timer <= 0) {
					// Randomize direction
					float random_x = (rand() % 200 - 100) / 100.f; // Random number between -1 and 1
					float random_y = (rand() % 200 - 100) / 100.f; // Random number between -1 and 1

					vec2 random_direction = normalize(vec2(random_x, random_y));
					motion.velocity = random_direction * 100.f;

					enemyAI.wander_timer = 2000.f;
				}
			}
		}
		else if (enemyAI.state == 1) {
			// if enemy's hp is lower than 30%, switch to fleeing
			if ((float)health.current_health / (float)health.max_health < 0.30) {
				enemyAI.state = 2;
				enemyAI.flee_timer = 3000.f;
				continue;
			}

			// move to the last position where the player is seen.
			vec2 direction = normalize(enemyAI.last_player_position - motion.position);
			motion.velocity = direction * 100.f;

			// if the player is outside of the radius for 3 seconds
			// return to the wander state
			if (distance(player_motion.position, motion.position) >= enemyAI.detection_radius) {
				enemyAI.chase_timer -= elapsed_ms_since_last_update;
				if (enemyAI.chase_timer <= 0) {
					enemyAI.state = 0;
					enemyAI.chase_timer = 3000.f;
				}
			}
			else { // else, update the position of the player for the AI
				enemyAI.last_player_position = player_motion.position;
				enemyAI.chase_timer = 3000.f;
			}

		}
		else if (enemyAI.state == 2) {
			// move away from the last position where the player is seen.
			vec2 direction = normalize(motion.position - enemyAI.last_player_position);
			motion.velocity = direction * 100.f;

			// if the player is outside of the radius for 3 seconds
			// return to the wander state
			if (distance(player_motion.position, motion.position) >= enemyAI.detection_radius) {
				enemyAI.flee_timer -= elapsed_ms_since_last_update;
				if (enemyAI.flee_timer <= 0) {
					enemyAI.state = 0;
					enemyAI.flee_timer = 3000.f;
				}
			}
			else { // else, update the position of the player for the AI
				enemyAI.last_player_position = player_motion.position;
				enemyAI.flee_timer = 3000.f;
			}
		}
	}
}