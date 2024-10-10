#include "game_scene.hpp"
#include "tiny_ecs_registry.hpp"
#include <iostream>


void GameScene::initialize(RenderSystem* renderer) {
	player = createPlayer(renderer, { 300, 300 });
	registry.colors.insert(player, { 1, 0.8f, 0.8f });
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

	//// Control direction of the salmon
	//if (action == GLFW_REPEAT && key == GLFW_KEY_UP) {
	//	Motion& motion = registry.motions.get(player);
	//	motion.position.y -= current_speed;
	//}

	//if (action == GLFW_REPEAT && key == GLFW_KEY_DOWN) {
	//	Motion& motion = registry.motions.get(player);
	//	motion.position.y += current_speed;
	//}

	//if (action == GLFW_REPEAT && key == GLFW_KEY_A) {
	//	Motion& motion = registry.motions.get(player);
	//	/*motion.position.x += current_speed * cos(motion.angle);
	//	motion.position.y -= current_speed * sin(motion.angle);*/
	//	motion.position.x -= current_speed;

	//	if (frame_counter++ >= frame_delay) {
	//		frame_counter = 0; // Reset counter

	//		// Update the texture based on the current frame in the subset
	//		auto& texture = registry.renderRequests.get(player);
	//		texture.used_texture = walking_sideways[frame];

	//		// Loop through the 3-frame subset
	//		frame = (frame + 1) % 3;
	//	}

	//	motion.scale.x = -abs(motion.scale.x);
	//}

	//if (action == GLFW_REPEAT && key == GLFW_KEY_D) {
	//	Motion& motion = registry.motions.get(player);
	//	/*motion.position.x += current_speed * cos(motion.angle);
	//	motion.position.y -= current_speed * sin(motion.angle);*/
	//	motion.position.x += current_speed;

	//	if (frame_counter++ >= frame_delay) {
	//		frame_counter = 0; // Reset counter

	//		// Update the texture based on the current frame in the subset
	//		auto& texture = registry.renderRequests.get(player);
	//		texture.used_texture = walking_sideways[frame];

	//		// Loop through the 3-frame subset
	//		frame = (frame + 1) % 3;
	//	}
	//	motion.scale.x = abs(motion.scale.x);
	//}

	//if (action == GLFW_REPEAT && key == GLFW_KEY_W) {
	//	Motion& motion = registry.motions.get(player);
	//	motion.position.y -= current_speed;

	//	if (frame_counter++ >= frame_delay) {
	//		frame_counter = 0; // Reset counter

	//		// Update the texture based on the current frame in the subset
	//		auto& texture = registry.renderRequests.get(player);
	//		texture.used_texture = walking_back[frame];

	//		// Loop through the 3-frame subset
	//		frame = (frame + 1) % 3;
	//	}
	//}

	//if (action == GLFW_REPEAT && key == GLFW_KEY_S) {
	//	Motion& motion = registry.motions.get(player);
	//	motion.position.y += current_speed;

	//	if (frame_counter++ >= frame_delay) {
	//		frame_counter = 0; // Reset counter

	//		// Update the texture based on the current frame in the subset
	//		auto& texture = registry.renderRequests.get(player);
	//		texture.used_texture = walking_front[frame];

	//		// Loop through the 3-frame subset
	//		frame = (frame + 1) % 3;
	//	}
	//}

	Motion& motion = registry.motions.get(player);
	auto& texture = registry.renderRequests.get(player);
	static bool isSprinting = false;

	// Handle movement keys (W, A, S, D)
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
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

	(int)key;
	(int)action;
	(int)mod;
	(RenderSystem*)renderer;
}

Entity GameScene::createPlayer(RenderSystem* renderer, vec2 pos)
{
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

	// create an empty Player component for our character
	Player& player = registry.players.emplace(entity);
	// Initialize health and ammo
	player.health = 3;
	player.ammo = 30;

	// Add the Health component to the player entity with initial health of 100
	Health& health = registry.healths.emplace(entity);
	health.current_health = 100;

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::PLAYER_1,
		//		{
		//			PLAYER_TEXTURE_ASSET_ID::PLAYER_1,
					// PLAYER_BACK_TEXTURE_ASSET_ID::TEXTURE_BACK_COUNT,
					EFFECT_ASSET_ID::TEXTURED,
					GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

