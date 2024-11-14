#include "game_scene.hpp"
#include "tiny_ecs_registry.hpp"
#include "maze.hpp" // Access box_testing_environment
#include "state_manager.hpp"
#include "physics_system.hpp" // to check_player_wall_collision
#include "render_system.hpp"
#include <iostream>
#include "components.hpp"

const int cell_size = 48;

// Debug Component
Entity createRing(vec2 position, vec2 scale)
{
	Entity entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	// Create motion
	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;
	motion.scale = scale;

	registry.debugComponents.emplace(entity);

	registry.renderRequests.insert(
		entity, {
			TEXTURE_ASSET_ID::TEXTURE_COUNT,
			EFFECT_ASSET_ID::RING,
			GEOMETRY_BUFFER_ID::DEBUG_LINE
		});

	return entity;
}

// Debug Component
Entity createBox(vec2 position, vec2 scale)
{
	Entity entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	// Create motion
	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;
	motion.scale = scale;

	registry.debugComponents.emplace(entity);

	registry.renderRequests.insert(
		entity, {
			TEXTURE_ASSET_ID::TEXTURE_COUNT,
			EFFECT_ASSET_ID::BOX,
			GEOMETRY_BUFFER_ID::DEBUG_LINE
		});

	return entity;
}

// Debug Component
Entity createInvisible(vec2 position)
{
	Entity entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	// Create motion
	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;
	motion.scale = {0, 0};

	registry.invisibles.emplace(entity);

	registry.renderRequests.insert(
		entity, {
			TEXTURE_ASSET_ID::TEXTURE_COUNT,
			EFFECT_ASSET_ID::BOX,
			GEOMETRY_BUFFER_ID::DEBUG_LINE
		});

	return entity;
}

Entity GameScene::createDirectionMarker(vec2 exit_position) {
	Entity entity = Entity();
	// computer marker location, clamped at the edge of the screen
	Motion& player_motion = registry.motions.get(player);
	vec2 direction = exit_position - player_motion.position;
	if (abs(direction.x) < ((float)window_width_px) / 2 && abs(direction.y) < ((float)window_height_px)/2) {
		return entity;
	}

	direction.x = min(max(direction.x, -((float)window_width_px) / 2), ((float)window_width_px) / 2);
	direction.y = min(max(direction.y, -((float)window_height_px) / 2), ((float)window_height_px) / 2);

	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = direction + vec2(((float)window_width_px) / 2, ((float)window_height_px) / 2);
	motion.scale = { 100, 100 };

	registry.UIs.emplace(entity);

	registry.renderRequests.insert(
		entity, {
			TEXTURE_ASSET_ID::TEXTURE_COUNT,
			EFFECT_ASSET_ID::BOX,
			GEOMETRY_BUFFER_ID::DEBUG_LINE
		});

	return entity;
}

Entity GameScene::createBackground() {
	RenderSystem* renderer = this->renderer;
	auto entity = Entity();

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = {window_width_px/2.f, window_height_px/2.f };
	motion.angle = 0;
	motion.velocity = { 0.f, 0.f };
	motion.scale = vec2({ window_width_px , window_height_px });

	registry.backgrounds.emplace(entity);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::SPACE_BACKGROUND,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}
//Entity createShadowOverlay(vec2 position) {
//
//}

// Create player hp bar
Entity GameScene::createPlayerHPBar(vec2 position, float ratio) {
	Entity entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;
	motion.scale = {200*ratio, 30};

	registry.UIs.emplace(entity);

	registry.renderRequests.insert(
		entity, {
			TEXTURE_ASSET_ID::TEXTURE_COUNT,
			EFFECT_ASSET_ID::BOX,
			GEOMETRY_BUFFER_ID::DEBUG_LINE
		});

	return entity;
}

// spawn enemies
void GameScene::spawnEnemiesAndItems() {
	std::cout << "spawned enemies" << std::endl;
	for (int row = 0; row < state.map_height; ++row) {
		for (int col = 0; col < state.map_width; ++col) {
			vec2 pos = { (col + 0.5) * state.TILE_SIZE, (row + 0.5) * state.TILE_SIZE };
			if (state.map[row][col] == 2) {
				Entity enemy = createEnemy(pos);
				registry.colors.insert(enemy, { 1, 0.8f, 0.8f });

				if (state.map_index == 1) { // tutorial 
					// Attach a hint to this enemy entity
					Hint hint;
					hint.text = "Left click to shoot!";
					hint.radius = 300.0f;  // Set the radius for the hint display
					registry.hints.emplace(enemy, hint);
				}
			}
			else if (state.map[row][col] == 3) {
				Entity chest = createHealthChest(pos);
				if (state.map_index == 1) {
					Hint hint;
					hint.text = "Chest with healing item. Press E to open";
					hint.radius = 200.0f;  // Set the radius for the hint display
					registry.hints.emplace(chest, hint);
				}
			}
			else if (state.map[row][col] == 4) {
				Entity chest = createAmmoChest(pos);
				if (state.map_index == 1) {
					Hint hint;
					hint.text = "Chest with ammo. Press E to open";
					hint.radius = 200.0f;  // Set the radius for the hint display
					registry.hints.emplace(chest, hint);
				}
			}
		}
	}

	// tutorial specific elements
	if (state.map_index == 1) {
		Entity marker = createInvisible({ 10, 10 });
		Hint hint1;
		hint1.text = "WASD to move player";
		hint1.radius = 300.0f;  // Set the radius for the hint display
		registry.hints.emplace(marker, hint1);

		marker = createInvisible({ (15+0.5)*48, (16 + 0.5) * 48 });
		Hint hint2;
		hint2.text = "Hold shift to sprint";
		hint2.radius = 300.0f;  // Set the radius for the hint display
		registry.hints.emplace(marker, hint2);
	}
	else {
		Entity marker = createInvisible({ 10, 10 });
		Hint hint3;
		hint3.text = "Look for the exit of the maze!";
		hint3.radius = 300.0f;  // Set the radius for the hint display
		registry.hints.emplace(marker, hint3);
	}

}

void GameScene::refreshUI(Entity player) {
	while (registry.UIs.entities.size() > 0)
		registry.remove_all_components_of(registry.UIs.entities.back());

	Player& player_component = registry.players.get(player);

	float ratio = ((float)player_component.health) / ((float)player_component.max_health);
	createPlayerHPBar({ 120.f - 100*(1-ratio), 35.f}, ratio);
	Entity health_text = renderer->text_renderer.createText(std::to_string(player_component.health) + "/" + std::to_string(player_component.max_health), { 35.f, 32.f }, 20.f, { 1.f, 1.f, 1.f });
	registry.UIs.emplace(health_text);

	// create ammo text
	Entity ammo_text = renderer->text_renderer.createText("Ammo: " + std::to_string(player_component.ammo), { 35.f, 72.f }, 20.f, { 1.f, 1.f, 1.f });
	registry.UIs.emplace(ammo_text);

	// Draw inventory slots
	createInventorySlots(player);

	//createDirectionMarker(vec2((state.current_map_state.exit.x + 0.5)*48, (state.current_map_state.exit.y + 0.5) * 48));
}


//void GameScene::updateHints(Entity player) {
//	vec2 player_position = registry.motions.get(player).position;
//
//	// Iterate over all entities with Hint components
//	for (Entity hint_entity : registry.hints.entities) {
//		// Check if the hint entity still has a valid Hint component
//		if (!registry.hints.has(hint_entity)) {
//			continue; // Skip this entity if it has been removed (e.g., enemy is dead)
//		}
//
//		Hint& hint = registry.hints.get(hint_entity);
//		vec2 hint_position = registry.motions.get(hint_entity).position;
//		/*std::cout << "Hint position: (" << hint_position.x << ", " << hint_position.y << ")" << std::endl;*/
//
//		// Check if player is within the hint radius
//		float distance = glm::distance(player_position, hint_position);
//		bool within_radius = distance <= hint.radius;
//
//		if (within_radius && !hint.is_visible) {
//			// Display hint text above the entity if within radius
//			hint.is_visible = true;
//			vec2 text_position = hint_position + vec2(0, 50);  // Adjust as needed for position above entity
//			/*vec2 text_position = { 1280 / 2.0f, 720 / 2.0f };*/
//			hint.text_entity = renderer->text_renderer.createText(hint.text, text_position, 20.f, { 1.f, 1.f, 1.f });
//		}
//		else if (!within_radius && hint.is_visible) {
//			// Hide hint text when outside radius
//			hint.is_visible = false;
//			renderer->text_renderer.removeText(hint.text_entity);  // Use the entity reference for removal
//			hint.text_entity = Entity();  // Reset to indicate no active text entity
//		}
//	}
//}

void GameScene::updateHints(Entity player) {
	vec2 player_position = registry.motions.get(player).position;

	// Iterate over all entities with Hint components
	for (Entity hint_entity : registry.hints.entities) {
		// Check if the hint entity still has a valid Hint component
		if (!registry.hints.has(hint_entity)) {
			continue; // Skip this entity if it has been removed (e.g., enemy is dead)
		}

		Hint& hint = registry.hints.get(hint_entity);
		vec2 hint_position = registry.motions.get(hint_entity).position;
		vec2 hint_scale = registry.motions.get(hint_entity).scale;

		// Check if player is within the hint radius
		float distance = glm::distance(player_position, hint_position);
		bool within_radius = distance <= hint.radius;

		// If within radius, display or update the hint text at a fixed screen position
		if (within_radius && !hint.is_visible) {
			hint.is_visible = true;

			// Calculate the screen position based on the world position with an offset above the entity
			//vec2 screen_position = camera_system.worldToView(hint_position + vec2(140, -30));
			hint.text_entity = renderer->text_renderer.createText(hint.text, hint_position + vec2(hint_scale.x, -30), 20.f, { 1.f, 1.f, 1.f });
			registry.cameraTexts.emplace(hint.text_entity);
		}
		// Hide the hint text if the player moves outside the hint radius
		else if (!within_radius && hint.is_visible) {
			hint.is_visible = false;
			renderer->text_renderer.removeText(hint.text_entity);
			hint.text_entity = Entity();  // Reset to indicate no active text entity
		}
		// If already visible and within radius, update the text position to keep it above the moving entity
		else if (within_radius && hint.is_visible) {
			//vec2 screen_position = camera_system.worldToView(hint_position + vec2(140, -30));
			renderer->text_renderer.updateTextPosition(hint.text_entity, hint_position + vec2(hint_scale.x, -30));
		}
	}
}

//bool show_bounding_boxes = true;
void GameScene::initialize(RenderSystem* renderer) {
	this->renderer = renderer;
	camera = renderer->getCameraSystem();

	// *Render the maze before initializing player and enemy entities*
	createBackground();
	std::string map_name = state.map_lists[state.map_index];
	MapState map_state = state.changeMap(map_name);
	createMaze(); 
	state.map_index++;
	createPortal({ (map_state.exit.x + 0.5) * state.TILE_SIZE, (map_state.exit.y + 0.5) * state.TILE_SIZE }, state.map_lists[state.map_index]);

	player = createPlayer({(map_state.player_spawn.x+0.5) * state.TILE_SIZE, (map_state.player_spawn.y+0.5) * state.TILE_SIZE }, selected_profession);
	registry.colors.insert(player, { 1, 0.8f, 0.8f });
	spawnEnemiesAndItems();

	//enemy = createEnemy({ 700, 300 });
	//registry.colors.insert(enemy, { 1, 0.8f, 0.8f });

	// fps entity
	FPS_entity = Entity();

	background_music = Mix_LoadMUS(audio_path("bgm.wav").c_str());
	player_dead_sound = Mix_LoadWAV(audio_path("death_sound.wav").c_str());
	player_hurt_sound = Mix_LoadWAV(audio_path("male-hurt.wav").c_str());
	monster_dead_sound = Mix_LoadWAV(audio_path("monster-dead.wav").c_str());
	monster_hurt_sound = Mix_LoadWAV(audio_path("monster-hurt.wav").c_str());

	explode_sound = Mix_LoadWAV(audio_path("explode.wav").c_str());
	shoot_sound = Mix_LoadWAV(audio_path("gunshot.wav").c_str());
	health_pickup_sound = Mix_LoadWAV(audio_path("health-pickup.wav").c_str());
	item_pickup_sound = Mix_LoadWAV(audio_path("item-pickup.wav").c_str());
	reload_sound = Mix_LoadWAV(audio_path("reload.wav").c_str());
	stab_sound = Mix_LoadWAV(audio_path("stab.wav").c_str());

	if (background_music == nullptr) {
		fprintf(stderr, "Failed to load sounds\n %s\n make sure the data directory is present",
			audio_path("bgm.wav").c_str());
		return;
	}
	if (player_dead_sound == nullptr) {
		fprintf(stderr, "Failed to load sounds\n %s\n make sure the data directory is present",
			audio_path("death_sound.wav").c_str());
		return;
	}

	Mix_VolumeMusic(10);
	// Playing background music indefinitely
	Mix_PlayMusic(background_music, -1);

	current_speed = 5.0f;
	player_velocity = { 0.0, 0.0 };

	refreshUI(player);
}

void GameScene::step(float elapsed_ms) {
	RenderSystem* renderer = this->renderer;
	auto& motion_container = registry.motions;
	// remove all debug component
	while (registry.debugComponents.entities.size() > 0)
		registry.remove_all_components_of(registry.debugComponents.entities.back());

	updateHints(player);

	// Update damage texts
	auto& damage_texts = registry.damageTexts;
	for (uint i = 0; i < damage_texts.components.size(); i++) {
		Entity entity = damage_texts.entities[i];
		DamageText& damage_text = damage_texts.components[i];

		// Decrease the duration
		damage_text.duration_ms -= 16.67f;  // Assuming 60 FPS

		// If the duration is over, remove the text entity
		if (damage_text.duration_ms <= 0) {
			registry.remove_all_components_of(entity);
		}
		else {
			// Adjust the position for effects if needed
			vec2 new_position = damage_text.position;
			renderer->setTextPosition(entity, new_position);
		}
	}

	if (debugging.in_debug_mode) {
		auto& enemyAI_container = registry.enemyAIs;
		for (uint i = 0; i < enemyAI_container.components.size(); i++) {
			EnemyAI enemyAI = enemyAI_container.components[i];
			Entity entity = enemyAI_container.entities[i];
			Motion motion = motion_container.get(entity);
			
			Entity ring = createRing(motion.position, vec2(enemyAI.detection_radius*2, enemyAI.detection_radius * 2));
			if (enemyAI.state == 0) {
				registry.colors.insert(ring, { 0.0, 1.0f, 0.0f });
			}
			else if (enemyAI.state == 1){
				registry.colors.insert(ring, { 1.0, 0.0f, 0.0f });
			}
			else if (enemyAI.state == 2) {
				registry.colors.insert(ring, { 0.0, 0.0f, 1.0f });
			}

			if (enemyAI.path.size() > 0) {
				for (vec2 path_node : enemyAI.path) {
					createBox((path_node + 0.5f) * 48.f, {10,10});
				}
			}
		}

		auto& bbox_container = registry.boundingBoxes;
		for (uint i = 0; i < bbox_container.components.size(); i++) {
			Entity entity = bbox_container.entities[i];
			Motion motion = motion_container.get(entity);

			createBox(motion.position, {10, 10});
		}
	}
	//drawHealthBars(renderer);
	// update LightUp timers and remove if time drops below zero, similar to the death counter
	for (Entity entity : registry.lightUps.entities) {
		// progress timer
		LightUp& lightup = registry.lightUps.get(entity);
		lightup.counter_ms -= elapsed_ms;

		/*std::cout << "LightUp timer: " << counter.counter_ms << " ms" << std::endl;*/

		// Interpolate the color and opacity using LERP: https://www.gamedev.net/tutorials/programming/general-and-gameplay-programming/a-brief-introduction-to-lerp-r4954/
		// function lerp(start, end, t)
		//    return start * (1 - t) + end * t;
		float t = (sin(lightup.blink_timer * 10) + 1) / 2;

		// Color shift: interpolate between the original color and the light up red color
		vec3 original_color = { 1.f, 1.f, 1.f }; // white
		vec3 lightup_color = { 1.f, 0.f, 0.f }; // red
		vec3 new_color = original_color * (1 - t) + lightup_color * t;	// lerp

		// Opacity shift: interpolate between the original opacity and 0.5
		float original_opacity = 1.f;
		float hit_opacity = 0.5f;
		float new_opacity = original_opacity * (1 - t) + hit_opacity * t;	// lerp

		// Update the color and opacity
		registry.colors.get(entity) = new_color;
		registry.opacities.get(entity).opacity = new_opacity;


		if (lightup.counter_ms < 0) {
			registry.lightUps.remove(entity); // remove the light up effect when timer ends
		}

		lightup.blink_timer += elapsed_ms / 1000.f; // update the blink timer
	}

	// Damage cool down timer drop 
	for (Entity entity : registry.damageCoolDowns.entities) {
		// progress timer
		DamageCoolDown& counter = registry.damageCoolDowns.get(entity);
		counter.counter_ms -= elapsed_ms;

		/*std::cout << "Damage timer: " << counter.counter_ms << " ms" << std::endl;*/

		if (counter.counter_ms < 0) {
			registry.damageCoolDowns.remove(entity);
		}

	}

	// FPS counter
	if (registry.fps.entities.size() == 0) {
		registry.fps.emplace(FPS_entity);
	}

	FPS& fps_counter = registry.fps.get(FPS_entity);

	fps_counter.elapsed_time += elapsed_ms;
	fps_counter.frame_count++;

	if (fps_counter.elapsed_time >= 1000) {
		fps_counter.fps = fps_counter.frame_count / (fps_counter.elapsed_time / 1000.f);
		fps_counter.frame_count = 0;
		fps_counter.elapsed_time = 0.0;
	}

	draw_fps();

	// Update HP and ammo
	refreshUI(player);

	// Update enemy hp bar
	auto& hp_bars = registry.healthBars;
	auto& healths = registry.healths;
	for (Entity entity : hp_bars.entities) {
		Motion& motion = motion_container.get(entity);
		HealthBar& hp_bar = hp_bars.get(entity);
		Motion& enemy_motion = motion_container.get(hp_bar.owner);
		Health& enemy_health = healths.get(hp_bar.owner);

		float ratio = 
			((float)enemy_health.current_health) / ((float)enemy_health.max_health);
		motion.position = enemy_motion.position + vec2(0, -50);
		motion.scale = { 60 * ratio, 10 };
	}


	//std::cout << "FPS: " << fps_counter.fps << std::endl;
	(RenderSystem*)renderer;

	//if (registry.enemies.size() > 0) {
	//	// Enemy walking frames and animations
	//	for (int i = 0; i < registry.enemies.size(); i++) {
	//		static int enemy_frame = 0;
	//		static float frame_delay = 150.f;
	//		static float frame_timer = 0.f;

	//		auto& enemy = registry.enemies.entities[i];
	//		auto& motion = registry.motions.get(enemy);

	//		TEXTURE_ASSET_ID enemy_walking_frames[4] = {
	//			TEXTURE_ASSET_ID::WOMAN_WALK_1,
	//			TEXTURE_ASSET_ID::WOMAN_WALK_2,
	//			TEXTURE_ASSET_ID::WOMAN_WALK_3,
	//			TEXTURE_ASSET_ID::WOMAN_WALK_4
	//		};

	//		frame_timer += elapsed_ms;
	//		if (frame_timer >= frame_delay) {
	//			frame_timer = 0.f;
	//			enemy_frame = (enemy_frame + 1) % 4;
	//		}

	//		auto& texture = registry.renderRequests.get(enemy);
	//		texture.used_texture = enemy_walking_frames[enemy_frame];

	//		static float last_direction_x = motion.velocity.x;
	//		static float flip_cooldown = 1000.f;
	//		static float flip_timer = 0.f;

	//		flip_timer += elapsed_ms;

	//		if (flip_timer >= flip_cooldown) {
	//			if (motion.velocity.x < 0 && last_direction_x >= 0) {
	//				motion.scale.x = -abs(motion.scale.x);
	//				last_direction_x = motion.velocity.x;
	//				flip_timer = 0.f; 
	//			}
	//			else if (motion.velocity.x > 0 && last_direction_x <= 0) {
	//				motion.scale.x = abs(motion.scale.x);
	//				last_direction_x = motion.velocity.x;
	//				flip_timer = 0.f;
	//			}
	//		}

	//		if (motion.velocity.x == 0) {
	//			texture.used_texture = TEXTURE_ASSET_ID::WOMAN_WALK_1;  // idle frame
	//		}

	//		// printf("enemy velocity: %f, last_direction: %f\n", motion.velocity.x, last_direction_x);
	//	}
	//}
	// deal with enemy death animation
	if (registry.enemyDeathTimers.size() > 0) {
		auto& enemyDeathTimers = registry.enemyDeathTimers;
		TEXTURE_ASSET_ID monster_dead[3] = {
			TEXTURE_ASSET_ID::WOMAN_DEAD_3,
			TEXTURE_ASSET_ID::WOMAN_DEAD_2,
			TEXTURE_ASSET_ID::WOMAN_DEAD_1
		};

		const float frame_delay = 1000.0f;

		for (Entity entity: enemyDeathTimers.entities) {
			// Entity entity = enemyDeathTimers.entities[i];
			EnemyDeathTime& enemyDeathTimer = enemyDeathTimers.get(entity);
			auto& motion = registry.motions.get(entity);
			motion.scale = abs(motion.scale);

			// Remove the enemy entity from the ai
			registry.enemyAIs.remove(entity);

			//printf("enemy position before: %f, %f\n", motion.position.x, motion.position.y);

			const float pos_x = motion.position.x;
			const float pos_y = motion.position.y;

			motion.position = { pos_x, pos_y };
			//printf("enemy position after: %f, %f\n", motion.position.x, motion.position.y);

			// Reduce timer for entity's lifespan
			enemyDeathTimer.counter_ms -= 20.0f;
			//printf("death timer: %f\n", enemyDeathTimer.counter_ms);

			// Calculate current frame based on elapsed time
			int frame = static_cast<int>((enemyDeathTimer.counter_ms) / frame_delay);
			//printf("current frame: %d\n", frame);

			// Check if entity's death animation has finished
			if (enemyDeathTimer.counter_ms < 0.f) {
				registry.remove_all_components_of(entity);
				//printf("Enemy is dead and removed!\n");
			}
			else {
				auto& texture = registry.renderRequests.get(entity);
				//printf("before scale: %f, %f\n", motion.scale.x, motion.scale.y);
				if (frame == 0) {
					motion.scale = { 1.547f * 64.f * 1.3, 1.547f * 10.f * 1.3 };
					motion.position.y += 3;
					//printf("after scale: %f, %f\n", motion.scale.x, motion.scale.y);
				}
				texture.used_texture = monster_dead[frame];
				//printf("current texture: %d\n", texture.used_texture);
			}
		}
	}

	// deal with player death animation
	assert(registry.screenStates.components.size() <= 1);
	ScreenState& screen = registry.screenStates.components[0];

	float min_counter_ms = 3000.f;
	for (Entity entity : registry.deathTimers.entities) {
		// progress timer
		DeathTimer& counter = registry.deathTimers.get(entity);
		counter.counter_ms -= elapsed_ms;
		if (counter.counter_ms < min_counter_ms) {
			min_counter_ms = counter.counter_ms;
		}
		screen.darken_screen_factor = 1 - min_counter_ms / 3000;

		// restart the game once the death timer expired
		if (counter.counter_ms < 0) {
			registry.deathTimers.remove(entity);
			screen.darken_screen_factor = 0;
			restart_game();
		}
	}

	// deal with player walking animation
	if (player_velocity.x != 0 || player_velocity.y != 0) {
		static int player_frame = 0;
		static float frame_delay = 150.f;
		static float frame_timer = 0.f;

		TEXTURE_ASSET_ID walking_sideways[3] = {
			TEXTURE_ASSET_ID::PLAYER_1,
			TEXTURE_ASSET_ID::PLAYER_2,
			TEXTURE_ASSET_ID::PLAYER_3
		};

		// Update the frame timer
		frame_timer += elapsed_ms;
		if (frame_timer >= frame_delay) {
			frame_timer = 0.f; // Reset the timer
			player_frame = (player_frame + 1) % 3; // Cycle through the frames
		}

		// Set the current walking frame texture
		auto& texture = registry.renderRequests.get(player);
		texture.used_texture = walking_sideways[player_frame];
	}
	// when player is not moving, set the texture to idle
	else {
		auto& texture = registry.renderRequests.get(player);
		texture.used_texture = TEXTURE_ASSET_ID::PLAYER_1;
	}
}

void GameScene::restart_game() {
	//printf("restarting\n");
	//while (registry.motions.entities.size() > 0)
	//	registry.remove_all_components_of(registry.motions.entities.back());

	//initialize(this->renderer);
	next_scene = "death_scene";
}

void GameScene::destroy() {
	while (registry.motions.entities.size() > 0)
		registry.remove_all_components_of(registry.motions.entities.back());
	while (registry.fps.entities.size() > 0)
		registry.remove_all_components_of(registry.fps.entities.back());
	Mix_FreeMusic(background_music);
	Mix_FreeChunk(player_dead_sound);
	Mix_FreeChunk(player_hurt_sound);
	Mix_FreeChunk(monster_dead_sound);
	Mix_FreeChunk(monster_hurt_sound);
	Mix_FreeChunk(explode_sound);
	Mix_FreeChunk(shoot_sound);
	Mix_FreeChunk(health_pickup_sound);
	Mix_FreeChunk(item_pickup_sound);
	Mix_FreeChunk(reload_sound);
	Mix_FreeChunk(stab_sound);
}

std::string GameScene::get_next_scene() {
	return this->next_scene;
}

void GameScene::on_key(int key, int action, int mod) {
	static int frame = 0;
	static int frame_counter = 0;
	static int first_round_frame_counter = 0;
	const int frame_delay = 2;
	TEXTURE_ASSET_ID walking_sideways[3] = {
	TEXTURE_ASSET_ID::PLAYER_1,
	TEXTURE_ASSET_ID::PLAYER_2,
	TEXTURE_ASSET_ID::PLAYER_3
	};

	Motion& motion = registry.motions.get(player);
	auto& texture = registry.renderRequests.get(player);
	static bool isSprinting = false;

	// Handle movement keys (W, A, S, D)
	if (!registry.deathTimers.has(player)) {
		if (action == GLFW_PRESS) {
			vec2 new_position = motion.position;
			if (action == GLFW_PRESS) {
				// Reset frame and counter when a new key is pressed
				frame = 0;
				frame_counter = 0;
			}
			switch (key) {
			case GLFW_KEY_W:
				player_velocity.y += -PLAYER_SPEED;
				texture.used_texture = walking_sideways[frame];
				break;
			case GLFW_KEY_S:
				player_velocity.y += PLAYER_SPEED;
				texture.used_texture = walking_sideways[frame];
				break;
			case GLFW_KEY_A:
				player_velocity.x += -PLAYER_SPEED;
				texture.used_texture = walking_sideways[frame];
				if (motion.scale.x > 0) {
					vec2 target_position = motion.position - motion.scale / 2.0f;
					updateCamera_smoothing(motion.position, target_position);
					Entity& gun = registry.guns.entities[0];
					registry.motions.get(gun);
					/*printf("gun position before: %f, %f\n", gun_motion.position.x, gun_motion.position.y);
					printf("player position before: %f, %f\n", motion.position.x, motion.position.y);*/

					Gun& gun_component = registry.guns.get(gun);
					gun_component.offset.x = -abs(gun_component.offset.x);
				}
				motion.scale.x = -abs(motion.scale.x); // Flip sprite to face left
				// printf("player velocity: %f, %f\n", player_velocity.x, player_velocity.y);
				break;
			case GLFW_KEY_D:
				player_velocity.x += PLAYER_SPEED;
				texture.used_texture = walking_sideways[frame];
				if (motion.scale.x < 0) {
					vec2 target_position = motion.position - motion.scale / 2.0f;
					updateCamera_smoothing(motion.position, target_position);

					Entity& gun = registry.guns.entities[0];
					registry.motions.get(gun);
					Gun& gun_component = registry.guns.get(gun);
					gun_component.offset.x = abs(gun_component.offset.x);
				}
				// printf("player velocity: %f, %f\n", player_velocity.x, player_velocity.y);
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
			case GLFW_KEY_F:
				// get the fps entity
				Entity fps_entity = registry.fps.entities[0];
				FPS& fps_counter = registry.fps.get(fps_entity);
				// visialize fps
				fps_counter.visible = !fps_counter.visible;
				printf("FPS counter visibility: %d\n", fps_counter.visible);
				printf("FPS: %f\n", fps_counter.fps);
				break;
			}

		}
		else if (action == GLFW_RELEASE) {
			switch (key) {
			case GLFW_KEY_W:
				player_velocity.y -= -PLAYER_SPEED;
				break;
			case GLFW_KEY_S:
				player_velocity.y -= PLAYER_SPEED;
				break;
			case GLFW_KEY_A:
				player_velocity.x -= -PLAYER_SPEED;
				break;
			case GLFW_KEY_D:
				player_velocity.x -= PLAYER_SPEED;
				break;
			case GLFW_KEY_LEFT_SHIFT:
				isSprinting = false;
				break;
			}

		}
		motion.velocity = player_velocity;

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
					// frame = (frame + 1) % 3;  // Increment frame and wrap around using modulus
					// texture.used_texture = walking_sideways[frame];
				}
				else if (key == GLFW_KEY_S) {
					/*frame = (frame + 1) % 3;
					texture.used_texture = walking_sideways[frame];*/
				}
				else if (key == GLFW_KEY_A || key == GLFW_KEY_D) {
					/*frame = (frame + 1) % 3;
					texture.used_texture = walking_sideways[frame];*/
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
	}

	// Handle interact key
	if (action == GLFW_RELEASE && key == GLFW_KEY_E) {
		// examine health chests
		auto& health_chests = registry.healthChests;
		for (Entity entity : health_chests.entities) {
			HealthChest& health_chest = health_chests.get(entity);
			Motion& motion = registry.motions.get(entity);
			Motion& player_motion = registry.motions.get(player);
			Player& player_component = registry.players.get(player);
			if (distance(motion.position, player_motion.position) < 200.f && !health_chest.isOpen) {
				Mix_PlayChannel(-1, health_pickup_sound, 0);
				player_component.health += (int)health_chest.amount;
				player_component.health = min(player_component.health, player_component.max_health);
				health_chest.isOpen = true;
				auto& chest_texture = registry.renderRequests.get(entity);
				chest_texture.used_texture = TEXTURE_ASSET_ID::CHEST_OPENED;
				if (registry.hints.has(entity)) {
					Hint& hint = registry.hints.get(entity);
					if (hint.is_visible) {
						// Remove the hint text entity if visible
						renderer->text_renderer.removeText(hint.text_entity);
					}
					registry.hints.remove(entity);
				}
			}
		}
		auto& ammo_chests = registry.ammoChests;
		for (Entity entity : ammo_chests.entities) {
			AmmoChest& ammo_chest = ammo_chests.get(entity);
			Motion& motion = registry.motions.get(entity);
			Motion& player_motion = registry.motions.get(player);
			Player& player_component = registry.players.get(player);
			if (distance(motion.position, player_motion.position) < 200.f && !ammo_chest.isOpen) {
				Mix_PlayChannel(-1, item_pickup_sound, 0);
				player_component.ammo += (int)ammo_chest.amount;
				ammo_chest.isOpen = true;
				auto& chest_texture = registry.renderRequests.get(entity);
				chest_texture.used_texture = TEXTURE_ASSET_ID::CHEST_OPENED;
				if (registry.hints.has(entity)) {
					Hint& hint = registry.hints.get(entity);
					if (hint.is_visible) {
						// Remove the hint text entity if visible
						renderer->text_renderer.removeText(hint.text_entity);
					}
					registry.hints.remove(entity);
				}
			}
		}
	}
	
	// Handle inventory slot usage with '1' '2' '3' '4'
	if (action == GLFW_PRESS) {
		if (key >= GLFW_KEY_1 && key <= GLFW_KEY_4) {
			int slot_index = key - GLFW_KEY_1;
			Entity player = registry.players.entities[0];
			Inventory& inventory = registry.inventories.get(player);

			if (slot_index < inventory.items.size() && inventory.items[slot_index].count > 0) {
				// Check item type and consume it
				if (inventory.items[slot_index].type == InventoryItem::Type::AmmoPack) {
					Player& player_component = registry.players.get(player);
					player_component.ammo += 10; // Increase ammo count
					inventory.items[slot_index].count--;

					// Play sound effect for item usage
					Mix_PlayChannel(-1, item_pickup_sound, 0);
				}
			}
		}
	}


	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_EQUAL:  // "+" key to zoom in
			camera.adjustZoom(0.1f);
			break;
		case GLFW_KEY_MINUS:  // "-" key to zoom out
			camera.adjustZoom(-0.1f);
			break;
		default:
			break;
		}
	}

	// Update the camera after input
	updateCamera(registry.motions.get(player).position);

	if (key == GLFW_KEY_SEMICOLON) {
		if (action == GLFW_RELEASE)
			debugging.in_debug_mode = false;
		else
			debugging.in_debug_mode = true;
	}

	
	(int)key;
	(int)action;
	(int)mod;
}

// Function to check if two bounding boxes overlap
bool GameScene::check_aabb_collision(const vec2& box1_min, const vec2& box1_max, const vec2& box2_min, const vec2& box2_max) {
	return (box1_min.x < box2_max.x &&
		box1_max.x > box2_min.x &&
		box1_min.y < box2_max.y &&
		box1_max.y > box2_min.y);
}

void GameScene::createMaze() {
	auto entity = Entity();
	Motion& motion = registry.motions.emplace(entity);
	motion.position = {24.f, 24.f};
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = {48.f, 48.f};

	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::TEXTURE_COUNT,
			EFFECT_ASSET_ID::MAP,
			GEOMETRY_BUFFER_ID::SPRITE
		});
	registry.maps.emplace(entity);
	std::cout << "called createMaze after: " << (unsigned int)entity << std::endl;

}


Entity GameScene::createWall(vec2 position, vec2 size)
{
	auto entity = Entity();

	// Motion component

	Motion& motion = registry.motions.emplace(entity);
	motion.position = position;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = size;


	// RenderRequest component

	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::WALL_6,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		});


	// Color component

	   // registry.colors.emplace(entity, vec3(0.f, 0.f, 0.f));  // Black color (RGB)

	registry.colors.emplace(entity, vec3(1.f, 1.f, 1.f));  // White color (RGB)


	return entity;
}

Entity GameScene::createPlayer(vec2 pos, std::string profession) {
	RenderSystem* renderer = this->renderer;
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::PLAYER);
	registry.meshPtrs.emplace(entity, &mesh);

	// Mesh original size : 0.009457, 0.017041
	printf("Player mesh original size: %f, %f\n", mesh.original_size.x, mesh.original_size.y);
	printf("Selected profession: %s\n", profession.c_str());
	// Adjusted the position of verticies to match the size of player in Mesh::loadFromOBJFile

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0;
	motion.velocity = { 0.f, 0.f };
	motion.scale = vec2({ 75.f , 75.f });

	// Create an empty Player component for our character
	Player& player = registry.players.emplace(entity);
	// Initialize health and ammo
	player.health = 20;
	player.ammo = 50;
	player.profession = profession;


	// Initialize player's inventory
	Inventory& inventory = registry.inventories.emplace(entity);
	inventory.items.resize(4); // 4 slots, all empty initially

	// Add an initial ammo pack to slot 1 for testing
	inventory.items[0] = { InventoryItem::Type::AmmoPack, 3 };



	// Add the Health component to the player entity with initial health of 100
	Health& health = registry.healths.emplace(entity);
	health.current_health = 100;

	

	// Add a bounding box to the player entity
	vec2 min = motion.position - (motion.scale / 2.0f);
	vec2 max = motion.position + (motion.scale / 2.0f);
	printf("Player bounding box min: (%f, %f)\n", min.x, min.y);
	printf("Player bounding box max: (%f, %f)\n", max.x, max.y);
	registry.boundingBoxes.emplace(entity, BoundingBox{ min, max });

	// Debug mode: visualize meshes
	if (debugging.in_debug_mode) {
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::PLAYER_1,
			  EFFECT_ASSET_ID::MESHED,
			  GEOMETRY_BUFFER_ID::PLAYER });
	}
	else {
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::PLAYER_1,
			  EFFECT_ASSET_ID::TEXTURED,
			  GEOMETRY_BUFFER_ID::SPRITE });
	}

	// Attach a gun to the player entity
	createGun(entity);

	return entity;
}

Entity GameScene::createChest(RenderSystem* renderer, vec2 position) {
    Entity chest_entity = Entity();

    // Store a reference to the potentially re-used mesh object (basic square for a chest)
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(chest_entity, &mesh);

    
    Motion& motion = registry.motions.emplace(chest_entity);
    motion.position = position;  
    motion.angle = 0.f;         
    motion.scale = vec2(TILE_SIZE, TILE_SIZE);  

    // Add render request for chest using a colored effect
    registry.renderRequests.insert(
        chest_entity,
        { TEXTURE_ASSET_ID::TEXTURE_COUNT,  // No texture, just use a color
          EFFECT_ASSET_ID::COLOURED,
          GEOMETRY_BUFFER_ID::SPRITE });

    // Set the color for the chest (yellow)
    registry.colors.emplace(chest_entity, vec3(1.0f, 1.0f, 0.0f));  // RGB for yellow

    return chest_entity;
}

Entity GameScene::createHealthChest(vec2 pos) {
	auto entity = Entity();


	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0;
	motion.velocity = { 0.f, 0.f };
	motion.scale = vec2({ 48.f ,48.f });

	registry.healthChests.emplace(entity);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::CHEST_CLOSED,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity GameScene::createAmmoChest(vec2 pos) {
	auto entity = Entity();


	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0;
	motion.velocity = { 0.f, 0.f };
	motion.scale = vec2({ 48.f ,48.f });

	registry.ammoChests.emplace(entity);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::CHEST_CLOSED,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity GameScene::createGun(Entity player) {
	RenderSystem* renderer = this->renderer;
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);
	
	// Create an empty Gun component for the gun
	Gun& gun = registry.guns.emplace(entity);
	gun.angle = 0.f;
	//gun.offset = { 20.f, 0.f };

	// Setting initial motion values
	Motion& gun_motion = registry.motions.emplace(entity);
	Motion& player_motion = registry.motions.get(player);
	gun_motion.position = player_motion.position + gun.offset;
	gun_motion.angle = 0;
	gun_motion.velocity = {0,0};
	gun_motion.scale = {35, 30};
	
	// gun's states
	gun.ammo_capacity = 30;
	gun.current_ammo = gun.ammo_capacity;
	gun.bullet_damage = 1;
	gun.bullet_speed = 100.f;
	gun.reload_time = 0.5f;
	gun.fire_rate = 0.5f;
	gun.direction = { 0.f, 0.f };

	// Add the parent component to the gun entity, linking it to the player
	registry.parents.emplace(entity, Parent{ player });


	// Add a bounding box to the gun entity, sync with player's bounding box size
	vec2 min = player_motion.position - (player_motion.scale / 2.0f);
	vec2 max = player_motion.position + (player_motion.scale / 2.0f);
	printf("Gun bounding box min: (%f, %f)\n", min.x, min.y);
	printf("Gun bounding box max: (%f, %f)\n", max.x, max.y);
	registry.boundingBoxes.emplace(entity, BoundingBox{ min, max });

	// Render request
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::SMG,
		  EFFECT_ASSET_ID::TEXTURED,
		  GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

//Entity GameScene::createHealthBar(RenderSystem* renderer, Entity entity, vec2 offset, vec2 size) {
//   auto health_bar_entity = Entity();
//
//    // Store a reference to the potentially re-used mesh object
//    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
//    registry.meshPtrs.emplace(health_bar_entity, &mesh);
//
//    // Get the entity's motion to determine where the health bar should be positioned
//    Motion& entity_motion = registry.motions.get(entity);
//
//    // Set initial motion values for the health bar
//    Motion& health_bar_motion = registry.motions.emplace(health_bar_entity);
//    health_bar_motion.position = entity_motion.position + offset;
//    health_bar_motion.angle = 0.f;
//    health_bar_motion.velocity = {0.f, 0.f};
//    health_bar_motion.scale = size;
//
//    // Add a HealthBar component to keep track of the owner entity
//    HealthBar& health_bar = registry.healthBars.emplace(health_bar_entity);
//    health_bar.owner = entity;
//
//    // Add the render request for the health bar (using a colored effect)
//    registry.renderRequests.insert(
//        health_bar_entity,
//        { TEXTURE_ASSET_ID::TEXTURE_COUNT,  // No texture needed, using a color
//          EFFECT_ASSET_ID::COLOURED,         // Use color to render the health bar
//          GEOMETRY_BUFFER_ID::SPRITE });
//
//    // Set the color of the health bar to red
//    registry.colors.emplace(health_bar_entity, vec3(1.0f, 0.0f, 0.0f));  // Red color
//
//    return health_bar_entity;
//
//}

Entity GameScene::createHealthBarNew(Entity enemy) {
	Entity entity = Entity();

	Motion& enemy_motion = registry.motions.get(enemy);
	Motion& motion = registry.motions.emplace(entity);
	vec2 pos = {0, -50};
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = pos + enemy_motion.position;
	motion.scale = { 60.f, 10 };

	HealthBar& hp_bar = registry.healthBars.emplace(entity);
	hp_bar.owner = enemy;

	registry.renderRequests.insert(
		entity, {
			TEXTURE_ASSET_ID::TEXTURE_COUNT,
			EFFECT_ASSET_ID::BOX,
			GEOMETRY_BUFFER_ID::DEBUG_LINE
		});

	return entity;
}

// Portal Component
Entity GameScene::createPortal(vec2 pos, std::string map_name) {
	Entity entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = pos;
	motion.scale = { 48, 48 };

	Portal& portal = registry.portals.emplace(entity);
	portal.next_map = map_name;

	vec2 min = motion.position - (motion.scale / 2.0f);
	vec2 max = motion.position + (motion.scale / 2.0f);
	registry.boundingBoxes.emplace(entity, BoundingBox{ min, max });

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::FLOOR_4,
		  EFFECT_ASSET_ID::TEXTURED,
		  GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity GameScene::createEnemy(vec2 pos) {
	RenderSystem* renderer = this->renderer;
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::ENEMY_WOMAN);
	registry.meshPtrs.emplace(entity, &mesh);

	// Mesh original size : 0.009997, 0.016473
	printf("Enemy mesh original size: %f, %f\n", mesh.original_size.x, mesh.original_size.y);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0;
	motion.velocity = { 0.f, 0.f };
	motion.scale = vec2({ 80.f ,80.f });

	// Create an empty Enemy component for the enemy character
	Enemy& enemy = registry.enemies.emplace(entity);

	// Add the Health component to the enemy entity with initial health of 50
	Health& health = registry.healths.emplace(entity);
	health.current_health = 10;
	health.max_health = 10;


	// health bar for enemy
	//vec2 offset = {0.f, -50.f};  // Position the health bar above the enemy
 //   vec2 size = {100.f, 10.f};   // Initial size of the health bar
 //   enemy.health_bar_entity = createHealthBar(renderer, entity, offset, size);
	
	// Ai timer for enemy
	AITimer& aiTimer = registry.aiTimers.emplace(entity);
	aiTimer.interval = 1000.f;
	aiTimer.counter_ms = 0.f;

	// Enemy AI
	registry.enemyAIs.emplace(entity);

	// Add a bounding box to the enemy entity
	vec2 min = motion.position - (motion.scale / 2.0f);
	vec2 max = motion.position + (motion.scale / 2.0f);
	printf("Enemy bounding box min: (%f, %f)\n", min.x, min.y);
	printf("Enemy bounding box max: (%f, %f)\n", max.x, max.y);
	registry.boundingBoxes.emplace(entity, BoundingBox{ min, max });

	if (debugging.in_debug_mode) {
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::WOMAN_WALK_1,
			  EFFECT_ASSET_ID::MESHED,
			  GEOMETRY_BUFFER_ID::ENEMY_WOMAN });
	}
	else {
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::WOMAN_WALK_1,
			  EFFECT_ASSET_ID::TEXTURED,
			  GEOMETRY_BUFFER_ID::SPRITE });
	}

	Entity hp_bar = createHealthBarNew(entity);

	enemy.health_bar_entity = hp_bar;

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
				//printf("Player health: %d\n", player.health);

				if (!registry.damageCoolDowns.has(entity) && !registry.deathTimers.has(entity)) {
					registry.damageCoolDowns.emplace(entity);
					player.health -= enemy.damage;

					if (player.health > 0) {
						Mix_PlayChannel(-1, player_hurt_sound, 0);
						// Mix_PlayChannel(-1, player_dead_sound, 0);
					}


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
							Mix_PlayChannel(-1, player_dead_sound, 0);
							registry.deathTimers.emplace(entity, DeathTimer{ 2000.f });
							// TODO: play death animation and restart the game
							// deal with player death animation
							Motion& motion = registry.motions.get(entity);
							motion.angle = M_PI / 2;
							registry.colors.get(entity) = { 1.f, 0.f, 0.f };
							motion.scale.x *= -1;
							motion.velocity = { 0, 0 };
						}
	
					}
				}
			}
		}

		// Bullet & Enemy collision
		if (registry.bullets.has(entity)) {
			if (registry.enemies.has(entity_other)) {
				// Bullet and enemy components
				Bullet& bullet = registry.bullets.get(entity);

				// Apply damage to the enemy
				apply_damage(entity_other, bullet.damage);

				// Remove the bullet after collision
				registry.remove_all_components_of(entity);
			}
		}

		//// Bullet & Wall collision check for each bullet in the registry
		//for (Entity bullet_entity : registry.bullets.entities) {
		//	Motion& bullet_motion = registry.motions.get(bullet_entity);
		//	vec2 bullet_min = bullet_motion.position - (bullet_motion.scale / 2.0f);
		//	vec2 bullet_max = bullet_motion.position + (bullet_motion.scale / 2.0f);

		//	// Loop through all entities with bounding boxes (assuming walls are part of this)
		//	for (uint i = 0; i < registry.boundingBoxes.components.size(); i++) {
		//		BoundingBox& wall_box = registry.boundingBoxes.components[i];
		//		vec2 wall_min = wall_box.min;
		//		vec2 wall_max = wall_box.max;

		//		// Check AABB collision between the bullet and wall
		//		if (check_aabb_collision(bullet_min, bullet_max, wall_min, wall_max)) {
		//			// Collision detected, remove the bullet
		//			registry.remove_all_components_of(bullet_entity);
		//			break;  // Exit inner loop once bullet is removed
		//		}
		//	}
		//}

		// Bullet & Enemy collision: Bullet hits the enemy, enemy loses 1 health, if health is 0, enemy dies
		// if (registry.bullets.has(entity)) {
		// if (registry.enemies.has(entity_other)) {
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

		// Player & Portal Collision
		if (registry.players.has(entity)) {
			if (registry.portals.has(entity_other)) {
				Portal& portal = registry.portals.get(entity_other);
				changeMap(portal.next_map);
			}
		}
	}

	// Remove all collisions from this simulation step
	registry.collisions.clear();
}

void GameScene::changeMap(std::string map_name) {
	if (state.map_index >= state.map_lists.size()) {
		next_scene = "over_scene";
		return;
	}
	state.map_index++;
	// remove bullets and enemies
	while (registry.hints.entities.size() > 0) {
		Entity& entity = registry.hints.entities.back();
		Hint& hint = registry.hints.get(entity);
		if (hint.is_visible) {
			// Remove the hint text entity if visible
			renderer->text_renderer.removeText(hint.text_entity);
		}
		// Clear the hint component from the entity
		registry.hints.remove(entity);
	}
	while (registry.bullets.entities.size() > 0)
		registry.remove_all_components_of(registry.bullets.entities.back());
	while (registry.enemies.entities.size() > 0)
		registry.remove_all_components_of(registry.enemies.entities.back());
	while (registry.healthBars.entities.size() > 0)
		registry.remove_all_components_of(registry.healthBars.entities.back());

	// remove hints
	while (registry.invisibles.entities.size() > 0)
		registry.remove_all_components_of(registry.invisibles.entities.back());

	// remove chests
	while (registry.healthChests.entities.size() > 0)
		registry.remove_all_components_of(registry.healthChests.entities.back());
	while (registry.ammoChests.entities.size() > 0)
		registry.remove_all_components_of(registry.ammoChests.entities.back());

	// also remove portals
	while (registry.portals.entities.size() > 0)
		registry.remove_all_components_of(registry.portals.entities.back());




	MapState map_state = state.changeMap(map_name);
	
	// spawn player
	Entity& player_entity = registry.players.entities[0];
	Motion& player_motion = registry.motions.get(player_entity);
	Player& player_component = registry.players.get(player_entity);
	player_component.health = player_component.max_health;
	player_component.ammo = 50;
	player_motion.position = { (map_state.player_spawn.x + 0.5) * 48,(map_state.player_spawn.y + 0.5) * 48 };
	// spawn exit
	if (state.map_index >= state.map_lists.size()) {
		createPortal({ (map_state.exit.x + 0.5) * state.TILE_SIZE, (map_state.exit.y + 0.5) * state.TILE_SIZE }, "n/a");
	}
	else {
		createPortal({ (map_state.exit.x + 0.5) * state.TILE_SIZE, (map_state.exit.y + 0.5) * state.TILE_SIZE }, state.map_lists[state.map_index]);
	}

	// spawn enemies
	spawnEnemiesAndItems();
}

// Add bullet creation
void GameScene::shoot_bullet(vec2 position, vec2 direction) {
	Player& player_component = registry.players.get(player);
	if (player_component.ammo <= 0) {
		return;
	}
	player_component.ammo -= 1;
	RenderSystem* renderer = this->renderer;
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::BULLET);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = position;
	motion.velocity = direction * 500.f;
	motion.angle = atan2(direction.y, direction.x); // Calculate angle from direction, in radians for sprite rotation
	motion.scale = vec2({ 15.f, 15.f });

	// Create an empty Bullet component for the bullet
	Bullet& bullet = registry.bullets.emplace(entity);

	// TODO: For bullet damage, we probably want to associate it with the gun, right now i kept it 1 for testing
	// //Associate bullet to gun
	//Entity gun = registry.guns.entities[0];
	//Gun& gun_component = registry.guns.get(gun);

	// //Set the bullet damage from the gun's bullet damage property
	//bullet.damage = gun_component.bullet_damage;

	bullet.damage = 1;
	bullet.speed = 100.f;
	bullet.direction = direction;

	// Associate bullet to gun
	Entity gun = registry.guns.entities[0];
	registry.parents.emplace(entity, Parent{ gun });

	// Gun's ammo - 1
	registry.guns.get(gun);
	//gun_component.current_ammo -= 1;

	// Add a bounding box to the bullet entity
	vec2 min = motion.position - (motion.scale / 2.0f);
	vec2 max = motion.position + (motion.scale / 2.0f);
	registry.boundingBoxes.emplace(entity, BoundingBox{ min, max });

	if (debugging.in_debug_mode) {
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::BULLET_1,
			  EFFECT_ASSET_ID::MESHED,
			  GEOMETRY_BUFFER_ID::BULLET });
	}
	else {
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::BULLET_1,
			  EFFECT_ASSET_ID::TEXTURED,
			  GEOMETRY_BUFFER_ID::SPRITE });
	}
  // Play the bullet fire sound
	Mix_PlayChannel(-1, shoot_sound, 0);
}

void GameScene::apply_damage(Entity& target, int damage) {
	// Check if the target has a Health component
	if (registry.healths.has(target)) {
		Mix_PlayChannel(-1, monster_dead_sound, 0);
		Health& health = registry.healths.get(target);
		health.current_health -= damage;  // Apply the damage
		std::cout << "Enemy lost " << damage << " health. Current health: " << health.current_health << std::endl;

		// Get the target position for the damage number display
		vec2 position = registry.motions.get(target).position;

		// Show the damage number at the target's position
		// show_damage_number(renderer, position, damage);

		// If health falls to 0 or below, remove the entity
		if (health.current_health <= 0) {
			// Check if the enemy has a hint component
			if (registry.hints.has(target)) {
				Hint& hint = registry.hints.get(target);
				if (hint.is_visible) {
					// Remove the hint text entity if visible
					renderer->text_renderer.removeText(hint.text_entity);
				}
				// Clear the hint component from the entity
				registry.hints.remove(target);
			}

			// Insert death animation timer or other removal actions
			// Remove the hp bar
			auto& enemy = registry.enemies.get(target);
			registry.remove_all_components_of(enemy.health_bar_entity);
			registry.enemies.remove(target);
			registry.enemyDeathTimers.insert(target, { 3000.0f, 3000.0f });
			Mix_PlayChannel(-1, monster_hurt_sound, 0);

			std::cout << "Enemy is dead!" << std::endl;
		}
	}
}

void GameScene::show_damage_number(vec2 position, int damage) {
	RenderSystem* renderer = this->renderer;
	// Create a text entity for the damage number
	auto entity = Entity();

	// Convert damage to string
	std::string damage_str = std::to_string(damage);

	// Create text component
	Text& damage_text = registry.texts.emplace(entity);
	damage_text.content = damage_str;

	// Set the initial position of the damage text
	DamageText& damage_text_component = registry.damageTexts.emplace(entity);
	damage_text_component.position = position;
	damage_text_component.duration_ms = 1000.f;  // Display for 1 second

	// Render the damage number
	vec2 text_position = position;  // You can modify this for effects, maybe damage text floats up in all other games
	Entity text_entity = renderer->text_renderer.createText(damage_str, text_position, 20.f, { 1.f, 0.f, 0.f });

	// Store the text entity in the registry
	registry.texts.emplace(entity, damage_text);
}

void GameScene::on_mouse_move(vec2 mouse_position) {
	/*if (registry.guns.entities.size() == 0) {
		return;
	}*/
	// Get the gun entity
	Entity entity = registry.guns.entities[0];

	// Handling gun rotation
	Motion& gun_motion = registry.motions.get(entity);

	// Calculate the direction vector from the gun to the mouse/cursor
	vec2 direction = mouse_position - vec2(window_width_px/2, window_height_px/2);

	// Calculate the rotation angle using atan2(y,x)
	gun_motion.angle = atan2(direction.y, direction.x);
	if (gun_motion.angle > (M_PIl / 2) || gun_motion.angle < -(M_PIl / 2)) {
		if (gun_motion.scale.y > 0) {
			gun_motion.scale.y = -gun_motion.scale.y;
		}
	}
	else {
		if (gun_motion.scale.y < 0) {
			gun_motion.scale.y = -gun_motion.scale.y;
		}
	}
}

void GameScene::on_mouse_click(int button, int action, int mod) {
	RenderSystem* renderer = this->renderer;
	if (registry.guns.entities.size() == 0) {
		return;
	}

	// Return if no ammo left
	Entity gun = registry.guns.entities[0];
	Gun& gun_component = registry.guns.get(gun);
	if (gun_component.current_ammo <= 0) {
		// TODO: reload logic
		return;
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		Motion& gun_motion = registry.motions.get(gun);

		// get the gun facing direction
		vec2 direction = { cos(gun_motion.angle), sin(gun_motion.angle) };

		// Normalize the direction vector, to ensure consistent bullet speed
		direction = normalize(direction);

		// Shoot a bullet
		shoot_bullet(gun_motion.position, direction);
	}

	(int)button;
	(int)action;
	(int)mod;
	(RenderSystem*)renderer;
}

void GameScene::draw_fps() {
	RenderSystem* renderer = this->renderer;
	if (registry.fpsTexts.entities.size() > 0) {
		registry.remove_all_components_of(registry.fpsTexts.entities.back());
	}
	Entity fps_entity = registry.fps.entities[0];
	FPS& fps_counter = registry.fps.get(fps_entity);
	if (fps_counter.visible) {
		std::string fps_string = "FPS: " + std::to_string(static_cast<int> (fps_counter.fps));

		// Update the text content
		//fps_text.content = fps_string;
		vec2 fps_position = vec2(10.f, 10.f);
		Entity text = renderer->text_renderer.createText(fps_string, fps_position, 20.f, { 0.f, 1.f, 0.f });
		registry.fpsTexts.emplace(text);
		//registry.cameraTexts.emplace(text);
	}
}


// void GameScene::drawHealthBars(RenderSystem* renderer) {


void GameScene::drawHealthBars() {
    auto& health_container = registry.healths;

    // Iterate over all entities with Health components
    for (uint i = 0; i < health_container.components.size(); i++) {
        Entity entity = health_container.entities[i];

        // Only update health bars for enemies
        if (registry.enemies.has(entity)) {
            Health& health = health_container.components[i];
            Motion& enemy_motion = registry.motions.get(entity);
            Enemy& enemy = registry.enemies.get(entity);

            // Calculate the health percentage
            float health_percentage = static_cast<float>(health.current_health) / static_cast<float>(health.max_health);
            vec2 size = {100.f * health_percentage, 10.f};  // Width proportional to health
            vec2 offset = {0.f, -50.f};  // Position the health bar above the enemy

            // Update the health bar's motion component
            if (enemy.health_bar_entity != Entity()) {
                Motion& health_bar_motion = registry.motions.get(enemy.health_bar_entity);
                health_bar_motion.position = enemy_motion.position + offset;
                health_bar_motion.scale = size;
            }
        }
    }
}

void GameScene::updateCamera(const vec2& player_position) {
	camera.updateCamera(player_position, window_width_px, window_height_px);
}

void GameScene::updateCamera_smoothing(const vec2& player_position, const vec2& target_position) {

	const float smooth_factor = 0.1f;  // Adjust this for smoother camera movement (0.0 - 1.0)

	// Smoothly move the camera towards the target position with a pivot correction
	vec2 pivot_corrected_position = player_position + smooth_factor * (target_position - player_position);
	camera.updateCamera(pivot_corrected_position, window_width_px, window_height_px);

}

// TODO: Reloading logic



// Inventory creation
void GameScene::createInventorySlots(Entity player) {
	Inventory& inventory = registry.inventories.get(player);

	float slot_size = 48.f;
	float spacing = 10.f;
	float x_offset = window_width_px / 2.0f - 2* slot_size;
	float y_offset = 50.f;

	for (int i = 0; i < inventory.max_slots; ++i) {
		float x_position = x_offset + i * (slot_size + spacing);
		vec2 position = { x_position, y_offset };

		// Create a slot entity
		Entity slot = Entity();
		Motion& motion = registry.motions.emplace(slot);
		motion.position = position;
		motion.scale = { slot_size, slot_size };
		registry.UIs.emplace(slot);

		// Render slot background
		registry.renderRequests.insert(slot, {
			TEXTURE_ASSET_ID::TEXTURE_COUNT,
			EFFECT_ASSET_ID::BOX,
			GEOMETRY_BUFFER_ID::DEBUG_LINE
			});

		// Render item icon if slot is not empty
		if (inventory.items[i].count > 0) {
			// Create a new icon entity for the item
			Entity icon = Entity();

			// Add a Motion component for position and scale
			Motion& icon_motion = registry.motions.emplace(icon);
			icon_motion.position = position; // Use screen position, not world position
			icon_motion.scale = { slot_size - 10, slot_size - 10 };

			// Mark this entity as a UI element so it renders on the UI layer
			registry.UIs.emplace(icon);

			// Render the icon with the appropriate texture
			registry.renderRequests.insert(icon, {
				TEXTURE_ASSET_ID::CHEST_CLOSED,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE
				});

			// Display the item count as text
			std::string count_text = std::to_string(inventory.items[i].count);
			renderer->text_renderer.createText(count_text, position + vec2(15, -15), 20.f, { 1.f, 1.f, 1.f });
		}
	}
}