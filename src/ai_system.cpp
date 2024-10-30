// internal
#include "ai_system.hpp"
#include <iostream>


void AISystem::step(float elapsed_ms_since_last_update)
{
	//for (Entity enemy : registry.enemies.entities) {
	//	Motion& motion = registry.motions.get(enemy);
	//	AITimer& aiTimer = registry.aiTimers.get(enemy);

	//	
	//	aiTimer.counter_ms += elapsed_ms_since_last_update;

	//	if (aiTimer.counter_ms >= aiTimer.interval) {
	//		aiTimer.counter_ms = 0.f;

	//		// Randomize direction
	//		float random_x = (rand() % 200 - 100) / 100.f; // Random number between -1 and 1
	//		float random_y = (rand() % 200 - 100) / 100.f; // Random number between -1 and 1

	//		vec2 random_direction = normalize(vec2(random_x, random_y));
	//		// Adjust speed
	//		motion.velocity = random_direction * 50.f; 
	//		//std::cout << "Enemy velocity: (" << motion.velocity.x << ", " << motion.velocity.y << ")" << std::endl;
	//	}
	//	motion.position += motion.velocity * (elapsed_ms_since_last_update)/1000.f;
	//}

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

		// states - root of the decision tree
		// wandering state: 0
		// chasing state: 1
		if (enemyAI.state == 0) {
			// if the player is within a certain distance, switch to chasing
			if (distance(player_motion.position, motion.position) <= enemyAI.detection_radius) {
				enemyAI.state = 1;
				enemyAI.wander_timer = 0;
			}
			// else, pick a random direction every certain interval
			else {
				enemyAI.wander_timer -= elapsed_ms_since_last_update;
				if (enemyAI.wander_timer <= 0) {
					// Randomize direction
					float random_x = (rand() % 200 - 100) / 100.f; // Random number between -1 and 1
					float random_y = (rand() % 200 - 100) / 100.f; // Random number between -1 and 1

					vec2 random_direction = normalize(vec2(random_x, random_y));
					motion.velocity = random_direction * 50.f;

					enemyAI.wander_timer = 2000.f;
				}
			}
		}
		else if (enemyAI.state == 1) {
			vec2 direction = normalize(player_motion.position - motion.position);
			motion.velocity = direction * 50.f;

			// if the player is outside of the radius for 3 seconds
			// return to the wander state
			if (distance(player_motion.position, motion.position) >= enemyAI.detection_radius) {
				enemyAI.chase_timer -= elapsed_ms_since_last_update;
				if (enemyAI.chase_timer <= 0) {
					enemyAI.state = 0;
					enemyAI.chase_timer = 3000.f;
				}
			}
			else {
				enemyAI.chase_timer = 3000.f;
			}

			// if the player is outside of the radius for 3 seconds
		}
	}
}