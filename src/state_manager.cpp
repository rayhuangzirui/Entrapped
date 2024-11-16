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
	map_height = map.size();
	map_width = map[0].size();

	current_map_state = map_state;

	return map_state;
}

//int GameScene::load_high_score() {
//	std::string filepath = std::string(PROJECT_SOURCE_DIR) + "highscore.txt";
//
//	FILE* file = fopen(filepath.c_str(), "r");
//	if (file == NULL) {
//		std::cerr << "Failed to open the file: " << filepath << std::endl;
//		return 0;
//	}
//
//	int content = 0;
//	// read the first word of the line
//	int res = fscanf(file, "%d", &content);
//	if (res == EOF) {
//		std::cerr << "Unable to read the high score" << std::endl;
//		return 0;
//	}
//	std::cout << "loaded high score: " << content << std::endl;
//	fclose(file);
//	return content;
//}


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

	std::cout << "Game loaded: " << std::endl;
	fclose(file);
	return;

}