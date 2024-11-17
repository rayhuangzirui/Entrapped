#include "state_manager.hpp"
#include "common.hpp"
#include <maze.hpp>
#include <tiny_ecs_registry.hpp>

StateManager state;

// returns player spawning position
MapState StateManager::changeMap(std::string map_name) {
	MapState map_state;
	map_state.map_name = map_name;
	if (map_name.compare("test") == 0) {
		map = box_testing_environtment_vector;

		map_state.player_spawn = { 5, 5 };
		map_state.exit = { 2, 2 };
	}
	else if (map_name.compare("tutorial") == 0) {
		map = tutorial_maze_vector;

		map_state.player_spawn = { 2,3 };
		map_state.exit = {2, 18};
		//map_state.exit = { 5, 3 };
	}
	else if (map_name.compare("map_1") == 0) {
		map = map_1_vector;

		map_state.player_spawn = { 3,3 };
		map_state.exit = { 58, 35 };
	}
	else if (map_name.compare("map_2") == 0) {
		map = map_2_vector;

		map_state.player_spawn = { 5,4 };
		map_state.exit = { 107, 44 };
	}
	map_height = map.size();
	map_width = map[0].size();

	current_map_state = map_state;

	return map_state;
}


void StateManager::save() {
	std::string filepath = std::string(PROJECT_SOURCE_DIR) + "save.dat";

	FILE* file = fopen(filepath.c_str(), "w");
	if (file == NULL) {
		std::cerr << "Failed to open the file: " << filepath << std::endl;
		return;
	}
	// player stats
	if (registry.players.size() > 0) {
		Player& p = registry.players.components[0];
		Entity& player = registry.players.entities[0];
		Inventory& inventory = registry.inventories.get(player);

		for (int i = 0; i < inventory.items.size(); i++) {
			InventoryItem& item = inventory.items[i];
			if (item.type == InventoryItem::Type::AmmoPack) {
				saved_ammo_pack = item.count;
			}
			else if (item.type == InventoryItem::Type::HealthPotion) {
				saved_health_potion = item.count;
			}
		}
		saved_health = p.health;
		saved_max_health = p.max_health;
		saved_ammo = p.ammo;
		saved_profession = p.profession;

		if (registry.speedBoosts.has(player)) {
			SpeedBoost& speed_boost = registry.speedBoosts.get(player);
			saved_speed_boost = speed_boost.count;
		}

		if (registry.shields.has(player)) {
			Shield& shield = registry.shields.get(player);
			saved_shield = shield.charges;
		}
	}

	std::cout << "Game saved" << std::endl;
	fprintf(file, "map_index=%d\n", map_index);
	fprintf(file, "exp=%d\n", exp);
	// health upgrades
	fprintf(file, "heath_upgrades=%d\n", health_upgrade.curVal);
	// ammo upgrades
	fprintf(file, "ammo_upgrades=%d\n", ammo_upgrade.curVal);
	fprintf(file, "health=%d\n", saved_health);
	fprintf(file, "max_health=%d\n", saved_max_health);
	fprintf(file, "ammo=%d\n", saved_ammo);
	fprintf(file, "ammo_pack=%d\n", saved_ammo_pack);
	fprintf(file, "health_potion=%d\n", saved_health_potion);
	fprintf(file, "speed_boost=%d\n", saved_speed_boost);
	fprintf(file, "shield=%d\n", saved_shield);

	// initialize player's powerup by the profession
	if (saved_profession == "Soldier") {
		fprintf(file, "profession=%d\n", 0);

	}
	else if (saved_profession == "Doctor") {
		fprintf(file, "profession=%d\n", 1);

	}
	else if (saved_profession == "Hacker") {
		fprintf(file, "profession=%d\n", 2);
	}
	fclose(file);
	return;

}

void StateManager::load() {
	std::string filepath = std::string(PROJECT_SOURCE_DIR) + "save.dat";

	FILE* file = fopen(filepath.c_str(), "r");
	if (file == NULL) {
		std::cerr << "Failed to open the file: " << filepath << std::endl;
		return;
	}

	int res = fscanf(file, "map_index=%d\n", &saved_map_index);
	if (res == EOF) {
		std::cerr << "Unable to read the save data" << std::endl;
		return;
	}

	res = fscanf(file, "exp=%d\n",&exp);
	if (res == EOF) {
		std::cerr << "Unable to read the save data" << std::endl;
		return;
	}

	res = fscanf(file, "heath_upgrades=%d\n", &health_upgrade.curVal);
	if (res == EOF) {
		std::cerr << "Unable to read the save data" << std::endl;
		return;
	}

	res = fscanf(file, "ammo_upgrades=%d\n", &ammo_upgrade.curVal);
	if (res == EOF) {
		std::cerr << "Unable to read the save data" << std::endl;
		return;
	}

	res = fscanf(file, "health=%d\n", &saved_health);
	if (res == EOF) {
		std::cerr << "Unable to read the save data" << std::endl;
		return;
	}

	res = fscanf(file, "max_health=%d\n", &saved_max_health);
	if (res == EOF) {
		std::cerr << "Unable to read the save data" << std::endl;
		return;
	}

	res = fscanf(file, "ammo=%d\n", &saved_ammo);
	if (res == EOF) {
		std::cerr << "Unable to read the save data" << std::endl;
		return;
	}
	// inventory
	res = fscanf(file, "ammo_pack=%d\n", &saved_ammo_pack);
	if (res == EOF) {
		std::cerr << "Unable to read the save data" << std::endl;
		return;
	}
	res = fscanf(file, "health_potion=%d\n", &saved_health_potion);
	if (res == EOF) {
		std::cerr << "Unable to read the save data" << std::endl;
		return;
	}
	// powerup
	res = fscanf(file, "speed_boost=%d\n", &saved_speed_boost);
	if (res == EOF) {
		std::cerr << "Unable to read the save data" << std::endl;
		return;
	}
	res = fscanf(file, "shield=%d\n", &saved_shield);
	if (res == EOF) {
		std::cerr << "Unable to read the save data" << std::endl;
		return;
	}

	int idx;
	res = fscanf(file, "profession=%d\n", &idx);
	if (res == EOF) {
		std::cerr << "Unable to read the save data" << std::endl;
		return;
	}
	// initialize player's powerup by the profession
	if (idx == 0) {
		saved_profession = "Soldier";
	}
	else if (idx == 1) {
		saved_profession = "Doctor";

	}
	else if (idx == 2) {
		saved_profession = "Hacker";
	}

	std::cout << "Game loaded: " << std::endl;
	fclose(file);
	return;

}

bool StateManager::is_blocked(int id) {
	return id == 1  // wall
		|| id == 3  // random chest
		|| id == 4  // health chest
		|| id == 5; // ammo chest
}

bool StateManager::is_floor(int id) {
	return id == 0  // floor
		|| id == 2  // enemy
		|| id == 3  // random chest
		|| id == 4  // health chest
		|| id == 5; // ammo chest
}

bool StateManager::is_wall(int id) {
	return id == 1; // wall
}