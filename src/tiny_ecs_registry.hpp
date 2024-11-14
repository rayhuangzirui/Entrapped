#pragma once
#include <vector>

#include "tiny_ecs.hpp"
#include "components.hpp"

class ECSRegistry
{
	// Callbacks to remove a particular or all entities in the system
	std::vector<ContainerInterface*> registry_list;

public:
	// Manually created list of all components this game has
	// TODO: A1 add a LightUp component
	ComponentContainer<LightUp> lightUps;

	ComponentContainer<DamageCoolDown> damageCoolDowns;
	ComponentContainer<Opacity> opacities;
	ComponentContainer<AITimer> aiTimers;
	ComponentContainer<Gun> guns;
	ComponentContainer<Parent> parents;
	ComponentContainer<FPS> fps;	

	ComponentContainer<DeathTimer> deathTimers;
	ComponentContainer<Motion> motions;
	ComponentContainer<Collision> collisions;
	ComponentContainer<Player> players;
	ComponentContainer<Mesh*> meshPtrs;
	ComponentContainer<RenderRequest> renderRequests;
	ComponentContainer<ScreenState> screenStates;
	ComponentContainer<DebugComponent> debugComponents;
	ComponentContainer<Text> texts;
	ComponentContainer<vec3> colors;
	ComponentContainer<Health> healths;  // Adding Health component container
	ComponentContainer<DashTimer> dashTimers;
	ComponentContainer<LightUp> lightups;
	ComponentContainer<EnemyDeathTime> enemyDeathTimers;
	ComponentContainer<HealthBar> healthBars;

	// AI related
	ComponentContainer<EnemyAI> enemyAIs;


	// Add bullet container and enemy container
	ComponentContainer<Bullet> bullets;
	ComponentContainer<Enemy> enemies;

	// Bounding box
	ComponentContainer<BoundingBox> boundingBoxes;

	//For Damage text render purpose
	ComponentContainer<DamageText> damageTexts;

	ComponentContainer<Map> maps;

	// UI related
	ComponentContainer<UI> UIs;
	ComponentContainer<PlayerHPBar> playerHPBars;
	ComponentContainer<FPSText> fpsTexts;
	ComponentContainer<Button> buttons;

	// Map transition
	ComponentContainer<Portal> portals;

	// Chest related
	ComponentContainer<HealthChest> healthChests;
	ComponentContainer<AmmoChest> ammoChests;

	// Hint related
	ComponentContainer<Hint> hints;
	ComponentContainer<CameraText> cameraTexts;
	ComponentContainer<Invisibles> invisibles;

	// Other functionality
	ComponentContainer<DirectionMarker> directionMarkers;

	ComponentContainer<Background> backgrounds;

	//Inventory Related
	ComponentContainer<InventoryItem> inventoryItems;
	ComponentContainer<Inventory> inventories;

	ComponentContainer<Refreshable> refreshables;


	// constructor that adds all containers for looping over them
	// IMPORTANT: Don't forget to add any newly added containers!
	ECSRegistry()
	{
		// TODO: A1 add a LightUp component
		registry_list.push_back(&lightUps);
		registry_list.push_back(&damageCoolDowns);
		registry_list.push_back(&opacities);
		registry_list.push_back(&aiTimers);
		registry_list.push_back(&guns);
		registry_list.push_back(&parents);

		registry_list.push_back(&deathTimers);
		registry_list.push_back(&motions);
		registry_list.push_back(&collisions);
		registry_list.push_back(&players);
		registry_list.push_back(&meshPtrs);
		registry_list.push_back(&renderRequests);
		registry_list.push_back(&screenStates);
		registry_list.push_back(&debugComponents);
		registry_list.push_back(&texts);
		registry_list.push_back(&colors);
		registry_list.push_back(&healths);	// Register the new Health component container
		registry_list.push_back(&dashTimers);
		// Add bullet container and enemy container to the registry
		registry_list.push_back(&bullets);
		registry_list.push_back(&enemies);
		registry_list.push_back(&lightups);
		registry_list.push_back(&enemyDeathTimers);
		registry_list.push_back(&boundingBoxes);
		// AI related
		registry_list.push_back(&enemyAIs);

		registry_list.push_back(&damageTexts);

		registry_list.push_back(&maps);

		// UI related
		registry_list.push_back(&UIs);
		registry_list.push_back(&refreshables);
		registry_list.push_back(&playerHPBars);
		registry_list.push_back(&fps);
		registry_list.push_back(&fpsTexts);
		registry_list.push_back(&buttons);

		// portal
		registry_list.push_back(&portals);

		// health bar
		registry_list.push_back(&healthBars);

		// chest
		registry_list.push_back(&healthChests);
		registry_list.push_back(&ammoChests);

		// hint
		registry_list.push_back(&hints);
		registry_list.push_back(&cameraTexts);
		registry_list.push_back(&invisibles);

		// direction marker
		registry_list.push_back(&directionMarkers);

		// background
		registry_list.push_back(&backgrounds);

		// Inventory
		registry_list.push_back(&inventoryItems);
		registry_list.push_back(&inventories);
	}

	void clear_all_components() {
		for (ContainerInterface* reg : registry_list)
			reg->clear();
	}

	void list_all_components() {
		printf("Debug info on all registry entries:\n");
		for (ContainerInterface* reg : registry_list)
			if (reg->size() > 0)
				printf("%4d components of type %s\n", (int)reg->size(), typeid(*reg).name());
	}

	void list_all_components_of(Entity e) {
		printf("Debug info on components of entity %u:\n", (unsigned int)e);
		for (ContainerInterface* reg : registry_list)
			if (reg->has(e))
				printf("type %s\n", typeid(*reg).name());
	}

	void remove_all_components_of(Entity e) {
		for (ContainerInterface* reg : registry_list)
			reg->remove(e);
	}
};

extern ECSRegistry registry;