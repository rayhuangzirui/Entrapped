#include "inventory_system.hpp"
#include "tiny_ecs_registry.hpp"
#include <SDL_mixer.h>
#include <iostream>

// Sound variables
Mix_Chunk* item_pickup_sound = nullptr;
Mix_Chunk* health_pickup_sound = nullptr;


// Function to initialize the player's inventory
void InventorySystem::initializeInventory(Entity player) {
    Inventory& inventory = registry.inventories.emplace(player);
    inventory.max_slots = 4;

    // Initialize inventory with empty items
    inventory.items.resize(inventory.max_slots);
    for (int i = 0; i < inventory.max_slots; ++i) {
        inventory.items[i] = { InventoryItem::Type::None, 0 };
    }

    // Debug message
    std::cout << "Initialized inventory for player with " << inventory.max_slots << " slots." << std::endl;
}

// Function to add an item to the player's inventory
bool InventorySystem::addItem(Entity player, InventoryItem::Type itemType, int count) {
    if (!registry.inventories.has(player)) return false;

    Inventory& inventory = registry.inventories.get(player);
    int original_count = count; // Track the initial count for logging

    // Check if there's an existing stack for this item
    for (auto& item : inventory.items) {
        if (item.type == itemType && item.count < item.max_count) {
            int space_left = item.max_count - item.count;
            int to_add = std::min(space_left, count);
            item.count += to_add;
            count -= to_add;

            // Log the addition
            std::cout << "Added " << to_add << " items to existing stack. Total in stack: " << item.count << std::endl;

            if (count == 0) {
                std::cout << "Successfully added " << original_count << " items in total." << std::endl;
                return true;
            }
        }
    }

    // If not, find an empty slot
    for (auto& item : inventory.items) {
        if (item.type == InventoryItem::Type::None) {
            item.type = itemType;
            item.count = count;
            std::cout << "Added " << count << " new items to an empty slot." << std::endl;
            return true;
        }
    }

    // Log if inventory is full
    std::cout << "Inventory full. Could only add " << (original_count - count)
        << " out of " << original_count << " items." << std::endl;
    return false;
}

// Function to remove an item from the player's inventory
bool InventorySystem::removeItem(Entity player, InventoryItem::Type itemType, int count) {
    if (!registry.inventories.has(player)) return false;

    Inventory& inventory = registry.inventories.get(player);

    for (auto& item : inventory.items) {
        if (item.type == itemType && item.count > 0) {
            // Check if the item has enough quantity to consume
            if (item.count >= count) {
                item.count -= count;
                int remaining = item.count;

                // Reset the type if count reaches zero
                if (item.count == 0) {
                    item.type = InventoryItem::Type::None;
                }

                // Output the results
                std::cout << "Removed " << count << " items of type " << static_cast<int>(itemType)
                    << ". Remaining: " << remaining << std::endl;
                return true;
            }
            else {
                // Not enough quantity to fully consume the requested amount
                std::cout << "Insufficient items to remove. Available: " << item.count
                    << ", requested: " << count << std::endl;
                return false;
            }
        }
    }

    std::cout << "Item of type " << static_cast<int>(itemType) << " not found in inventory." << std::endl;
    return false;
}

// Function to consume an item from a specific slot
void InventorySystem::consumeItem(Entity player, int slot_index) {
    if (!registry.inventories.has(player)) return;

    Inventory& inventory = registry.inventories.get(player);

    // Check if the slot index is valid
    if (slot_index < 0 || slot_index >= inventory.items.size()) {
        std::cout << "Invalid inventory slot." << std::endl;
        return;
    }

    InventoryItem& item = inventory.items[slot_index];

    // Check if the slot contains an item
    if (item.type == InventoryItem::Type::None || item.count <= 0) {
        std::cout << "No item in this slot to consume." << std::endl;
        return;
    }

    // Consume the item based on its type
    switch (item.type) {
    case InventoryItem::Type::AmmoPack:
        consumeAmmoPack(player);
        break;
    case InventoryItem::Type::HealthPotion:
        consumeHealthPotion(player);
        break;
    default:
        std::cout << "Unknown item type." << std::endl;
        return;
    }

    // Deduct the item count
    item.count--;

    // If the count reaches zero, clear the slot
    if (item.count == 0) {
        item.type = InventoryItem::Type::None;
    }

    std::cout << "Consumed 1 item from slot " << slot_index << ". Remaining: " << item.count << std::endl;
}

// Function to add ammo to the player
void InventorySystem::consumeAmmoPack(Entity player) {
    if (!registry.players.has(player)) return;

    Player& player_component = registry.players.get(player);
    player_component.ammo += 10;

    // Play the item pickup sound
    if (item_pickup_sound)
        Mix_PlayChannel(-1, item_pickup_sound, 0);

    std::cout << "Added 10 ammo. Current ammo: " << player_component.ammo << std::endl;
}

// Function to restore health to the player
void InventorySystem::consumeHealthPotion(Entity player) {
    if (!registry.players.has(player)) return;

    Player& player_component = registry.players.get(player);
    player_component.health = std::min(player_component.health + 10, player_component.max_health);

    if (health_pickup_sound)
        Mix_PlayChannel(-1, health_pickup_sound, 0);

    std::cout << "Restored 10 health. Current health: " << player_component.health << std::endl;
}


void InventorySystem::initializeSounds() {
    item_pickup_sound = Mix_LoadWAV(audio_path("item-pickup.wav").c_str());
    if (item_pickup_sound == nullptr) {
        std::cerr << "Failed to load item-pickup.wav: " << Mix_GetError() << std::endl;
    }

    health_pickup_sound = Mix_LoadWAV(audio_path("health-pickup.wav").c_str());
    if (health_pickup_sound == nullptr) {
        std::cerr << "Failed to load health-pickup.wav: " << Mix_GetError() << std::endl;
    }
}

void InventorySystem::cleanupSounds() {
    if (item_pickup_sound) {
        Mix_FreeChunk(item_pickup_sound);
        item_pickup_sound = nullptr;
    }
    if (health_pickup_sound) {
        Mix_FreeChunk(health_pickup_sound);
        health_pickup_sound = nullptr;
    }
}


TEXTURE_ASSET_ID getItemTextureID(InventoryItem::Type itemType) {
    static const std::unordered_map<InventoryItem::Type, TEXTURE_ASSET_ID> itemTextures = {
        { InventoryItem::Type::AmmoPack, TEXTURE_ASSET_ID::CHEST_CLOSED },
        { InventoryItem::Type::HealthPotion, TEXTURE_ASSET_ID::CHEST_OPENED }//,
        //{ InventoryItem::Type::SpeedBoost, TEXTURE_ASSET_ID::SPEED_ICON },
        //{ InventoryItem::Type::Shield, TEXTURE_ASSET_ID::SHIELD_ICON }
    };

    auto it = itemTextures.find(itemType);
    if (it != itemTextures.end()) {
        return it->second;
    }
    return TEXTURE_ASSET_ID::TEXTURE_COUNT; // Default texture for unknown items
}