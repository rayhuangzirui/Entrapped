// internal
#include "ai_system.hpp"
#include <iostream>


void AISystem::step(float elapsed_ms_since_last_update)
{
	for (Entity enemy : registry.enemies.entities) {
		Motion& motion = registry.motions.get(enemy);
		AITimer& aiTimer = registry.aiTimers.get(enemy);

		
		aiTimer.counter_ms += elapsed_ms_since_last_update;

		if (aiTimer.counter_ms >= aiTimer.interval) {
			aiTimer.counter_ms = 0.f;

			// Randomize direction
			float random_x = (rand() % 200 - 100) / 100.f; // Random number between -1 and 1
			float random_y = (rand() % 200 - 100) / 100.f; // Random number between -1 and 1

			vec2 random_direction = normalize(vec2(random_x, random_y));
			// Adjust speed
			motion.velocity = random_direction * 50.f; 
			//std::cout << "Enemy velocity: (" << motion.velocity.x << ", " << motion.velocity.y << ")" << std::endl;
		}
		motion.position += motion.velocity * (elapsed_ms_since_last_update)/1000.f;
	}
}