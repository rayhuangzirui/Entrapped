#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"

//For wall
#include "render_system.hpp"

// Use createSalmon for creating the player
Entity createPlayer(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	//Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::PLAYER);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0;
	motion.velocity = { 0.f, 0.f };
	//motion.scale = vec2({ PLAYER_BB_WIDTH, PLAYER_BB_HEIGHT });
	motion.scale = mesh.original_size;

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

Entity createEnemy(RenderSystem* renderer, vec2 position)
{

	// auto entity = Entity();

	// // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	// Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	// registry.meshPtrs.emplace(entity, &mesh);

	// // Initialize the motion
	// auto& motion = registry.motions.emplace(entity);
	// motion.angle = 0.f;
	// motion.velocity = { 0, 100.f };
	// motion.position = position;

	// // Setting initial values, scale is negative to make it face the opposite way
	// motion.scale = vec2({ -EEL_BB_WIDTH, EEL_BB_HEIGHT });

	// // create an empty Eel component to be able to refer to all eels
	// registry.deadlys.emplace(entity);
	// registry.renderRequests.insert(
	// 	entity,
	// 	{
	// 		TEXTURE_ASSET_ID::EEL,
	// 		EFFECT_ASSET_ID::TEXTURED,
	// 		GEOMETRY_BUFFER_ID::SPRITE
	// 	});

	// return entity;
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::ENEMY_WOMAN);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 100.f }; 
	motion.position = position;

	motion.scale = vec2({ENEMY_BB_WIDTH, ENEMY_BB_WIDTH });

	// create an empty Eel component to be able to refer to all eels
	Enemy& enemy = registry.enemies.emplace(entity);
	enemy.health = 2;
	enemy.damage = 1;

	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::WOMAN_WALK_1,
			// PLAYER_TEXTURE_ASSET_ID::PLAYER_1,
			// PLAYER_BACK_TEXTURE_ASSET_ID::TEXTURE_BACK_COUNT,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		});

	return entity;

}

// Debug Component
Entity createLine(vec2 position, vec2 scale)
{
	Entity entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	registry.renderRequests.insert(
		entity, {
			TEXTURE_ASSET_ID::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no txture is needed
			EFFECT_ASSET_ID::DEBUG_LINE,	// BULLET is a simple shader that renders the object in a single color
			GEOMETRY_BUFFER_ID::DEBUG_LINE // DEBUG_LINE is a 1x1 unit square
		});

	// Create motion
	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;
	motion.scale = scale;

	registry.debugComponents.emplace(entity);
	return entity;
}


Entity createWall(RenderSystem* renderer, vec2 position, vec2 size)
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
                TEXTURE_ASSET_ID::TEXTURE_COUNT,
                EFFECT_ASSET_ID::COLOURED,
                GEOMETRY_BUFFER_ID::SQUARE
            });
    

    // Color component
   
       // registry.colors.emplace(entity, vec3(0.f, 0.f, 0.f));  // Black color (RGB)

	   registry.colors.emplace(entity, vec3(1.f, 1.f, 1.f));  // White color (RGB)
    

    return entity;
}


Entity createBackground(RenderSystem* renderer) {
	auto entity = Entity();

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = { window_width_px / 2.f, window_height_px / 2.f };
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

Entity createChooseProfessionTitle(RenderSystem* renderer) {
	auto entity = Entity();

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = { window_width_px / 2.f, 100.f };
	motion.angle = 0;
	motion.velocity = { 0.f, 0.f };
	motion.scale = vec2({ 800 , 300 });

	registry.backgrounds.emplace(entity);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::CHOOSE_PROFESSION_TITLE,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createProfessionLayer(RenderSystem* renderer, vec2 pos, std::string profession, bool is_clicked) {
	auto entity = Entity();

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0;
	motion.velocity = { 0.f, 0.f };
	motion.scale = vec2({ 550 , 550 });

	registry.backgrounds.emplace(entity);

	if (profession == "Soldier") {
		if (!is_clicked) {
			registry.renderRequests.insert(
				entity,
				{ TEXTURE_ASSET_ID::SOLDIER_PAGE,
					EFFECT_ASSET_ID::TEXTURED,
					GEOMETRY_BUFFER_ID::SPRITE });
		}
		else {
			registry.renderRequests.insert(
				entity,
				{ TEXTURE_ASSET_ID::SOLDIER_PAGE_CLICKED,
					EFFECT_ASSET_ID::TEXTURED,
					GEOMETRY_BUFFER_ID::SPRITE });
		}
	}


	return entity;
}

// create a confirm button
Entity createConfirmButton(RenderSystem* renderer, vec2 pos, bool is_enabled, bool is_clicked) {
	auto entity = Entity();

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0;
	motion.velocity = { 0.f, 0.f };
	motion.scale = vec2({ 550 , 550 }); //165, 82.5

	registry.backgrounds.emplace(entity);

	if (is_enabled) {
		if (is_clicked) {
			registry.renderRequests.insert(
				entity,
				{ TEXTURE_ASSET_ID::CONFIRM_BUTTON_CLICKED,
					EFFECT_ASSET_ID::TEXTURED,
					GEOMETRY_BUFFER_ID::SPRITE });
		}
		else {
			registry.renderRequests.insert(
				entity,
				{ TEXTURE_ASSET_ID::CONFIRM_BUTTON,
					EFFECT_ASSET_ID::TEXTURED,
					GEOMETRY_BUFFER_ID::SPRITE });
		}
	}
	else {
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::CONFIRM_BUTTON_DISABLED,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
	}

	return entity;
}


