#include "state_manager.hpp"
#include "common.hpp"
#include <maze.hpp>

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
	std::cout << "Game saved" << std::endl;
	fprintf(file, "map_index=%d\n", map_index);
	fprintf(file, "exp=%d\n", exp);
	// health upgrades
	fprintf(file, "heath_upgrades=%d\n", health_upgrade.curVal);
	// ammo upgrades
	fprintf(file, "ammo_upgrades=%d\n", ammo_upgrade.curVal);
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