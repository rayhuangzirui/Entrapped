#include "power_up_system.hpp"
#include "components.hpp"
#include "tiny_ecs_registry.hpp"
#include "render_system.hpp"
#include <iostream>

// Function to apply a power-up to a player
// Function to apply a power-up to a player
void PowerUpSystem::applyPowerUp(Entity player, PowerUpType type, int strength) {
    if (type == PowerUpType::Shield) {
        // If the player already has a shield, add to the existing shield count
        if (registry.shields.has(player)) {
            Shield& shield = registry.shields.get(player);
            shield.charges += strength;

            // Debug: Print the updated number of shield charges
            std::cout << "[DEBUG] Player collected a shield power-up! Total shields: "
                << shield.charges << std::endl;
        }
        else {
            // Add a new Shield component with the initial charge
            Shield& shield = registry.shields.emplace(player);
            shield.charges = strength;

            // Debug: Print the number of shields after adding the first one
            std::cout << "[DEBUG] Player collected the first shield power-up! Total shields: "
                << shield.charges << std::endl;
        }
    }
}


// Function to handle power-up collection
void PowerUpSystem::collectPowerUp(Entity power_up_entity, Entity player) {
    PowerUp& power_up = registry.powerUps.get(power_up_entity);

    switch (power_up.type) {
    case PowerUpType::Shield:
        applyPowerUp(player, PowerUpType::Shield, 1);
        break;
    default:
        break;
    }

    // Remove the power-up entity after collection
    registry.remove_all_components_of(power_up_entity);
}

// Function to update power-up effects
void PowerUpSystem::updatePowerUps(float elapsed_ms) {
    // Example: Handle time-limited power-ups in the future
}
