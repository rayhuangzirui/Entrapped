#include "game_scene.hpp"
#include "tiny_ecs_registry.hpp"
#include <iostream>


void GameScene::initialize(RenderSystem* renderer) {
	player = createPlayer(renderer, { 500, 500 });
	registry.colors.insert(player, { 1, 0.8f, 0.8f });
	current_speed = 10.0f;
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
	const int frame_delay = 5;
	PLAYER_TEXTURE_ASSET_ID walking_sideways[3] = {
	PLAYER_TEXTURE_ASSET_ID::PLAYER_1,
	PLAYER_TEXTURE_ASSET_ID::PLAYER_2,
	PLAYER_TEXTURE_ASSET_ID::PLAYER_3
	};

	PLAYER_TEXTURE_ASSET_ID walking_front[3] = {
	PLAYER_TEXTURE_ASSET_ID::PLAYER_FRONT_1,
	PLAYER_TEXTURE_ASSET_ID::PLAYER_FRONT_2,
	PLAYER_TEXTURE_ASSET_ID::PLAYER_FRONT_3
	};

	PLAYER_TEXTURE_ASSET_ID walking_back[3] = {
	PLAYER_TEXTURE_ASSET_ID::PLAYER_BACK_1,
	PLAYER_TEXTURE_ASSET_ID::PLAYER_BACK_2,
	PLAYER_TEXTURE_ASSET_ID::PLAYER_BACK_3
	};

	// Control direction of the salmon
	if (action == GLFW_REPEAT && key == GLFW_KEY_UP) {
		Motion& motion = registry.motions.get(player);
		motion.position.y -= current_speed;
	}

	if (action == GLFW_REPEAT && key == GLFW_KEY_DOWN) {
		Motion& motion = registry.motions.get(player);
		motion.position.y += current_speed;
	}

	if (action == GLFW_REPEAT && key == GLFW_KEY_A) {
		Motion& motion = registry.motions.get(player);
		/*motion.position.x += current_speed * cos(motion.angle);
		motion.position.y -= current_speed * sin(motion.angle);*/
		motion.position.x -= current_speed;

		if (frame_counter++ >= frame_delay) {
			frame_counter = 0; // Reset counter

			// Update the texture based on the current frame in the subset
			auto& texture = registry.renderRequests.get(player);
			texture.used_texture = walking_sideways[frame];

			// Loop through the 3-frame subset
			frame = (frame + 1) % 3;
		}

		motion.scale.x = -abs(motion.scale.x);
	}

	if (action == GLFW_REPEAT && key == GLFW_KEY_D) {
		Motion& motion = registry.motions.get(player);
		/*motion.position.x += current_speed * cos(motion.angle);
		motion.position.y -= current_speed * sin(motion.angle);*/
		motion.position.x += current_speed;

		if (frame_counter++ >= frame_delay) {
			frame_counter = 0; // Reset counter

			// Update the texture based on the current frame in the subset
			auto& texture = registry.renderRequests.get(player);
			texture.used_texture = walking_sideways[frame];

			// Loop through the 3-frame subset
			frame = (frame + 1) % 3;
		}
		motion.scale.x = abs(motion.scale.x);
	}

	if (action == GLFW_REPEAT && key == GLFW_KEY_W) {
		Motion& motion = registry.motions.get(player);
		motion.position.y -= current_speed;

		if (frame_counter++ >= frame_delay) {
			frame_counter = 0; // Reset counter

			// Update the texture based on the current frame in the subset
			auto& texture = registry.renderRequests.get(player);
			texture.used_texture = walking_back[frame];

			// Loop through the 3-frame subset
			frame = (frame + 1) % 3;
		}
	}

	if (action == GLFW_REPEAT && key == GLFW_KEY_S) {
		Motion& motion = registry.motions.get(player);
		motion.position.y += current_speed;

		if (frame_counter++ >= frame_delay) {
			frame_counter = 0; // Reset counter

			// Update the texture based on the current frame in the subset
			auto& texture = registry.renderRequests.get(player);
			texture.used_texture = walking_front[frame];

			// Loop through the 3-frame subset
			frame = (frame + 1) % 3;
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
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SALMON);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = mesh.original_size * 500.f;
	motion.scale.x *= 1; // point front to the right

	// create an empty Salmon component for our character
	registry.players.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ PLAYER_TEXTURE_ASSET_ID::PLAYER_1, // TEXTURE_COUNT indicates that no texture is needed
		// PLAYER_BACK_TEXTURE_ASSET_ID::TEXTURE_BACK_COUNT,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });
	std::cout << "player successfully created" << std::endl;
	return entity;
}

