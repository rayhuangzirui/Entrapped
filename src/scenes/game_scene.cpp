#include "game_scene.hpp"
#include "tiny_ecs_registry.hpp"
#include "maze.hpp" // Access box_testing_environment
#include "physics_system.hpp" // to check_player_wall_collision
#include "render_system.hpp"
#include <iostream>

//bool show_bounding_boxes = true;
void GameScene::initialize(RenderSystem* renderer) {

	// *Render the maze before initializing player and enemy entities*
	render_maze(renderer);

	player = createPlayer(renderer, { 300, 300 });
	registry.colors.insert(player, { 1, 0.8f, 0.8f });

	enemy = createEnemy(renderer, { 700, 300 });
	registry.colors.insert(enemy, { 1, 0.8f, 0.8f });

	// Render bounding boxes for debugging (if enabled)
	//if (show_bounding_boxes) {
	//	render_bounding_boxes(renderer);
	//}

	current_speed = 5.0f;


	(RenderSystem*)renderer;
}

void GameScene::step(RenderSystem* renderer) {
	(RenderSystem*)renderer;
}

void GameScene::destroy(RenderSystem* renderer) {
	while (registry.motions.entities.size() > 0)
		registry.remove_all_components_of(registry.motions.entities.back());
	(RenderSystem*)renderer;
}

void GameScene::on_key(RenderSystem* renderer, int key, int action, int mod) {
	static int frame = 0;
	static int frame_counter = 0;
	static int first_round_frame_counter = 0;
	const int frame_delay = 2;
	TEXTURE_ASSET_ID walking_sideways[3] = {
	TEXTURE_ASSET_ID::PLAYER_1,
	TEXTURE_ASSET_ID::PLAYER_2,
	TEXTURE_ASSET_ID::PLAYER_3
	};

	TEXTURE_ASSET_ID walking_front[3] = {
	TEXTURE_ASSET_ID::PLAYER_FRONT_1,
	TEXTURE_ASSET_ID::PLAYER_FRONT_2,
	TEXTURE_ASSET_ID::PLAYER_FRONT_3
	};

	TEXTURE_ASSET_ID walking_back[3] = {
	TEXTURE_ASSET_ID::PLAYER_BACK_1,
	TEXTURE_ASSET_ID::PLAYER_BACK_2,
	TEXTURE_ASSET_ID::PLAYER_BACK_3
	};

	Motion& motion = registry.motions.get(player);
	auto& texture = registry.renderRequests.get(player);
	static bool isSprinting = false;

	// Handle movement keys (W, A, S, D)
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		vec2 new_position = motion.position;
		if (action == GLFW_PRESS) {
			// Reset frame and counter when a new key is pressed
			frame = 0;
			frame_counter = 0;
		}

		switch (key) {
		case GLFW_KEY_W:
			motion.velocity.y = -PLAYER_SPEED;
			texture.used_texture = walking_back[frame];
			break;
		case GLFW_KEY_S:
			motion.velocity.y = PLAYER_SPEED;
			texture.used_texture = walking_front[frame];
			break;
		case GLFW_KEY_A:
			motion.velocity.x = -PLAYER_SPEED;
			texture.used_texture = walking_sideways[frame];
			motion.scale.x = -abs(motion.scale.x); // Flip sprite to face left
			break;
		case GLFW_KEY_D:
			motion.velocity.x = PLAYER_SPEED;
			texture.used_texture = walking_sideways[frame];
			motion.scale.x = abs(motion.scale.x); // Ensure sprite faces right
			break;
		case GLFW_KEY_LEFT_SHIFT:
			isSprinting = true;
			break;
		case GLFW_KEY_SPACE:
			if (!registry.dashTimers.has(player)) {
				registry.dashTimers.emplace(player, DashTimer{ 200.f });
				motion.velocity *= 2.5f;
			}
			break;
		}

	}
	else if (action == GLFW_RELEASE) {
		switch (key) {
		case GLFW_KEY_W:
		case GLFW_KEY_S:
			motion.velocity.y = 0.f;
			break;
		case GLFW_KEY_A:
		case GLFW_KEY_D:
			motion.velocity.x = 0.f;
			break;
		case GLFW_KEY_LEFT_SHIFT:
			isSprinting = false;
			break;
		}
	}

	// Apply sprint effect if active
	if (isSprinting) {
		if (motion.velocity.x != 0) {
			motion.velocity.x = (motion.velocity.x > 0) ? PLAYER_SPEED * 2 : -PLAYER_SPEED * 2;
		}
		if (motion.velocity.y != 0) {
			motion.velocity.y = (motion.velocity.y > 0) ? PLAYER_SPEED * 2 : -PLAYER_SPEED * 2;
		}
	}
	else {
		if (motion.velocity.x > PLAYER_SPEED) {
			motion.velocity.x = PLAYER_SPEED;
		}
		else if (motion.velocity.x < -PLAYER_SPEED) {
			motion.velocity.x = -PLAYER_SPEED;
		}
		if (motion.velocity.y > PLAYER_SPEED) {
			motion.velocity.y = PLAYER_SPEED;
		}
		else if (motion.velocity.y < -PLAYER_SPEED) {
			motion.velocity.y = -PLAYER_SPEED;
		}
	}

	// Update frame and texture for animation when moving
	if (action == GLFW_REPEAT) {
		// Increment the frame after the delay to create animation effect
		if (frame_counter++ >= frame_delay) {
			frame_counter = 0;

			// Loop through the frames for the current direction
			if (key == GLFW_KEY_W) {
				frame = (frame + 1) % 3;  // Increment frame and wrap around using modulus
				texture.used_texture = walking_back[frame];  // Set texture for walking back
			}
			else if (key == GLFW_KEY_S) {
				frame = (frame + 1) % 3;
				texture.used_texture = walking_front[frame];  // Set texture for walking front
			}
			else if (key == GLFW_KEY_A || key == GLFW_KEY_D) {
				frame = (frame + 1) % 3;
				texture.used_texture = walking_sideways[frame];  // Set texture for walking sideways
			}

			// Adjust sprite orientation for left or right movement
			if (key == GLFW_KEY_A) {
				motion.scale.x = -abs(motion.scale.x);  // Face left
			}
			else if (key == GLFW_KEY_D) {
				motion.scale.x = abs(motion.scale.x);  // Face right
			}
		}
	}

	// After updating player position:
	if (registry.boundingBoxes.has(player)) {
		Motion& motion = registry.motions.get(player);
		BoundingBox& bounding_box = registry.boundingBoxes.get(player);
		bounding_box.min = motion.position - (motion.scale / 2.0f);
		bounding_box.max = motion.position + (motion.scale / 2.0f);
		printf("Bounding box min: (%f, %f)\n", bounding_box.min.x, bounding_box.min.y);
		printf("Bounding box max: (%f, %f)\n", bounding_box.max.x, bounding_box.max.y);
	}

	(int)key;
	(int)action;
	(int)mod;
	(RenderSystem*)renderer;
}

// Function to check if two bounding boxes overlap
bool GameScene::check_aabb_collision(const vec2& box1_min, const vec2& box1_max, const vec2& box2_min, const vec2& box2_max) {
	return (box1_min.x < box2_max.x &&
		box1_max.x > box2_min.x &&
		box1_min.y < box2_max.y &&
		box1_max.y > box2_min.y);
}

// TODO: Remove this grid collision detection when aabb_collision is finished
bool GameScene::check_player_wall_collision(const Motion& player_motion) {
    // Calculate the player's bounding box for the new position
    vec2 player_min = player_motion.position - (player_motion.scale / 2.0f);
    vec2 player_max = player_motion.position + (player_motion.scale / 2.0f);

    // Loop through all entities with bounding boxes (walls)
    for (uint i = 0; i < registry.boundingBoxes.components.size(); i++) {
        BoundingBox& bounding_box = registry.boundingBoxes.components[i];
        vec2 wall_min = bounding_box.min;
        vec2 wall_max = bounding_box.max;

        // Use AABB collision check
        if (check_aabb_collision(player_min, player_max, wall_min, wall_max)) {
            return true; // Collision detected
        }
    }

    return false; // No collision detected
}


// Currently only using box_testing_environment in maze.cpp, change variable names accordingly if you want to render another maze
void GameScene::render_maze(RenderSystem* renderer) {
	// Tile dimensions
	const int TILE_SIZE = 48;

	// Loop through each row and column of the maze
	for (int row = 0; row < BOX_MAZE_HEIGHT; ++row) {
		for (int col = 0; col < BOX_MAZE_WIDTH; ++col) {
			// Determine the texture to use and create an entity for the tile
			TEXTURE_ASSET_ID texture_id;
			if (box_testing_environment[row][col] == 1) {
				texture_id = TEXTURE_ASSET_ID::WALL_6;

				// Calculate position for the tile
				float x = col * TILE_SIZE;
				float y = row * TILE_SIZE;

				// Create an entity for the wall tile
				Entity wall_entity = Entity();

				// Store a reference to the potentially re-used mesh object
				Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
				registry.meshPtrs.emplace(wall_entity, &mesh);

				// Setting initial motion values
				Motion& motion = registry.motions.emplace(wall_entity);
				motion.position = { x, y };
				motion.angle = 0;
				motion.scale = { TILE_SIZE, TILE_SIZE };

				// Add a bounding box component for wall tiles
				vec2 min = vec2(x, y);
				vec2 max = vec2(x + TILE_SIZE, y + TILE_SIZE);
				registry.boundingBoxes.emplace(wall_entity, BoundingBox{ min, max });

				// Add the render request for the wall entity
				registry.renderRequests.insert(
					wall_entity,
					{ texture_id,
					  EFFECT_ASSET_ID::TEXTURED,
					  GEOMETRY_BUFFER_ID::SPRITE });
			}
			else {
				texture_id = TEXTURE_ASSET_ID::FLOOR_5;

				// Calculate position for the tile
				float x = col * TILE_SIZE;
				float y = row * TILE_SIZE;

				// Create an entity for the floor tile
				Entity floor_entity = Entity();

				// Store a reference to the potentially re-used mesh object
				Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
				registry.meshPtrs.emplace(floor_entity, &mesh);

				// Setting initial motion values
				Motion& motion = registry.motions.emplace(floor_entity);
				motion.position = { x, y };
				motion.angle = 0;
				motion.scale = { TILE_SIZE, TILE_SIZE };

				// Add the render request for the floor entity
				registry.renderRequests.insert(
					floor_entity,
					{ texture_id,
					  EFFECT_ASSET_ID::TEXTURED,
					  GEOMETRY_BUFFER_ID::SPRITE });
			}
		}
	}
}


Entity GameScene::createPlayer(RenderSystem* renderer, vec2 pos) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0;
	motion.velocity = { 0.f, 0.f };
	motion.scale = vec2({ 0.6f * 165.f , 0.6f * 165.f });

	// Create an empty Player component for our character
	Player& player = registry.players.emplace(entity);
	// Initialize health and ammo
	player.health = 3;
	player.ammo = 30;

	// Add the Health component to the player entity with initial health of 100
	Health& health = registry.healths.emplace(entity);
	health.current_health = 100;

	// Add a bounding box to the player entity
	vec2 min = motion.position - (motion.scale / 2.0f);
	vec2 max = motion.position + (motion.scale / 2.0f);
	printf("Bounding box min: (%f, %f)\n", min.x, min.y);
	printf("Bounding box max: (%f, %f)\n", max.x, max.y);
	registry.boundingBoxes.emplace(entity, BoundingBox{ min, max });

	// Add the render request for the player entity
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::PLAYER_1,
		  EFFECT_ASSET_ID::TEXTURED,
		  GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity GameScene::createEnemy(RenderSystem* renderer, vec2 pos) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0;
	motion.velocity = { 0.f, 0.f };
	motion.scale = vec2({ 0.6f * 165.f , 0.6f * 165.f });

	// Create an empty Enemy component for the enemy character
	Enemy& enemy = registry.enemies.emplace(entity);

	// Add the Health component to the enemy entity with initial health of 50
	Health& health = registry.healths.emplace(entity);
	health.current_health = 10;

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::WOMAN_WALK_1,
		  EFFECT_ASSET_ID::TEXTURED,
		  GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

// Compute collisions between entities
void GameScene::handle_collisions() {
	// Loop over all collisions detected by the physics system
	auto& collisionsRegistry = registry.collisions;
	for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
		// The entity and its collider
		Entity entity = collisionsRegistry.entities[i];
		Entity entity_other = collisionsRegistry.components[i].other;

		// Player & Enemy collision: Enemy attacks the player, player loses 1 health, if health is 0, player dies
		if (registry.players.has(entity)) {
			if (registry.enemies.has(entity_other)) {
				// Player and enemy components
				Player& player = registry.players.get(entity);
				Enemy& enemy = registry.enemies.get(entity_other);

				// Reduce player health
				printf("Player health: %d\n", player.health);

				if (!registry.damageCoolDowns.has(entity)) {
					registry.damageCoolDowns.emplace(entity);
					player.health -= enemy.damage;


					if (!registry.lightUps.has(entity)) {
						registry.lightUps.emplace(entity);
					}

					if (!registry.opacities.has(entity)) {
						registry.opacities.emplace(entity, Opacity{ 0.5f });
					}

					if (player.health <= 0) {
						//registry.colors.get(entity) = { 1.f, 0.f, 0.f };
						//registry.remove_all_components_of(entity);
						
						if (!registry.deathTimers.has(entity)) {
							registry.deathTimers.emplace(entity, DeathTimer{ 2000.f });
							// TODO: play death animation and restart the game
						}
						
					}
				}
			}
		}

		// Bullet & Enemy collision: Bullet hits the enemy, enemy loses 1 health, if health is 0, enemy dies
		//if (registry.bullets.has(entity)) {
		//	if (registry.enemies.has(entity_other)) {
		//		// Bullet and enemy components
		//		Bullet& bullet = registry.bullets.get(entity);
		//		Enemy& enemy = registry.enemies.get(entity_other);

		//		// Reduce enemy health
		//		enemy.health -= bullet.damage;

		//		// Red tint light up effect on enemy
		//		registry.lightUps.emplace(entity_other);
		//		//registry.colors.get(entity_other) = { 1.f, 0.f, 0.f }; // Red tint

		//		// Play the bullet hit sound
		//		//Mix_PlayChannel(-1, bullet_hit_sound, 0);

		//		// Check if the enemy is dead
		//		if (enemy.health <= 0) {
		//			// TODO: play the enemy dead animation

		//			// Play the enemy dead sound
		//			//Mix_PlayChannel(-1, enemy_dead_sound, 0);

		//			// Remove the enemy
		//			registry.remove_all_components_of(entity_other);
		//		}

		//		// Remove the bullet
		//		registry.remove_all_components_of(entity);
		//	}
		//}
	}

	// Remove all collisions from this simulation step
	registry.collisions.clear();
}

