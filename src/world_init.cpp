#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"

// Use createSalmon for creating the player
Entity createPlayer(RenderSystem* renderer, vec2 pos)
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
	motion.scale = vec2({ PLAYER_BB_WIDTH, PLAYER_BB_HEIGHT });

	// create an empty Player component for our character
	Player& player = registry.players.emplace(entity);
	// Initialize health and ammo
	player.health = 3;
	player.ammo = 30;

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::PLAYER,
//		{
//			PLAYER_TEXTURE_ASSET_ID::PLAYER_1,
			// PLAYER_BACK_TEXTURE_ASSET_ID::TEXTURE_BACK_COUNT,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createEnemy(RenderSystem* renderer, vec2 position)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
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
			TEXTURE_ASSET_ID::ENEMY, 
			// PLAYER_TEXTURE_ASSET_ID::PLAYER_1,
			// PLAYER_BACK_TEXTURE_ASSET_ID::TEXTURE_BACK_COUNT,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		});

	return entity;
}

Entity createLine(vec2 position, vec2 scale)
{
	Entity entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	registry.renderRequests.insert(
		entity, {
			TEXTURE_ASSET_ID::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no txture is needed
			EFFECT_ASSET_ID::BULLET,	// BULLET is a simple shader that renders the object in a single color
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

Entity createBullet(RenderSystem* renderer, vec2 pos, vec2 vel)
{
	auto entity = Entity();
	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::BULLET);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = vel; 
	motion.scale = vec2({BULLET_BB_WIDTH, BULLET_BB_HEIGHT});

	// create an empty component for bullet
	Bullet& bullet = registry.bullets.emplace(entity);
	// Initialize damage
	bullet.damage = 1;

	registry.renderRequests.insert(
		entity, {
			TEXTURE_ASSET_ID::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no txture is needed
			EFFECT_ASSET_ID::BULLET,
			GEOMETRY_BUFFER_ID::BULLET
		});

	return entity;
}

