#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"

//For wall
#include "render_system.hpp"

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

Entity createButton(RenderSystem* renderer, vec2 pos, vec2 scale, std::string text) {
	Entity entity = Entity();

	// Create motion
	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	vec2 position = pos;
	position.x += (scale.x / 2.f - 15.f);
	motion.position = position;
	motion.scale = scale;

	registry.UIs.emplace(entity);
	Button& button = registry.buttons.emplace(entity);

	registry.renderRequests.insert(
		entity, {
			TEXTURE_ASSET_ID::TEXTURE_COUNT,
			EFFECT_ASSET_ID::BOX,
			GEOMETRY_BUFFER_ID::DEBUG_LINE
		});

	registry.colors.insert(entity, { 1.0f, 0.0f, 0.0f });

	Entity text_entity = renderer->text_renderer.createText(text, pos, 20.f, { 1.f, 1.f, 1.f });
	button.text = text_entity;

	return entity;
}
