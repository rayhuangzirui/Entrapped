#pragma once
#include "tiny_ecs_registry.hpp"
#include "components.hpp"

// System to handle power-up logic
class PowerUpSystem {
public:
    static void applyPowerUp(Entity player, PowerUpType type, int strength);
    static void updatePowerUps(float elapsed_ms);
    static void collectPowerUp(Entity power_up_entity, Entity player);
};
