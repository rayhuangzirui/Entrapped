#pragma once
#include <vector>
#include "common.hpp"

class StateManager
{
public:
	const int TILE_SIZE = 48;
	int map_height;
	int map_width;
	std::vector<std::vector<int>> map;

	StateManager()
	{
		map_height = 3;
		map_width = 3;
		map = {
			{1, 2, 3},
			{4, 5, 6},
			{7, 8, 9}
		};
	}

	vec2 changeMap(std::string map_name);
};

extern StateManager state;