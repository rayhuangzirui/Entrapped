#pragma once
#include "common.hpp"
#include "tiny_ecs_registry.hpp"

// Inventory system class
class InventorySystem {
public:
    static void initializeInventory(Entity player);
    static bool addItem(Entity player, InventoryItem::Type itemType, int count);
    static bool removeItem(Entity player, InventoryItem::Type itemType, int count);

    // Function to consume an item from a specific inventory slot
    static void consumeItem(Entity player, int slot_index);

    // Behavior functions for different item types
    static void consumeAmmoPack(Entity player);
    static void consumeHealthPotion(Entity player);

    // Sounds
    static void initializeSounds();
    static void cleanupSounds();

};
