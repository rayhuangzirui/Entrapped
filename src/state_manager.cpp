#include "state_manager.hpp"
#include "common.hpp"
#include <maze.hpp>

StateManager state;

// returns player spawning position
vec2 StateManager::changeMap(std::string map_name) {
	if (map_name.compare("test") == 0) {
		map = box_testing_environtment_vector;
		map_height = map.size();
		map_width = map[0].size();

		return { 4, 4 };
	}
	else if (map_name.compare("tutorial") == 0) {
		map = tutorial_maze_vector;
		map_height = map.size();
		map_width = map[0].size();

		return { 2, 3 };
	}

	return { 0, 0 };
}