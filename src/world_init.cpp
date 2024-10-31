#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"

//For wall
#include "render_system.hpp"

// Debug Component
Entity createLine(vec2 position, vec2 scale)
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

