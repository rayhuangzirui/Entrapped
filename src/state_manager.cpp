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