#pragma once
#include <vector>
#include "common.hpp"

struct MapState {
	std::string map_name = "test";
	vec2 player_spawn = { 5, 5 };
	vec2 exit = { 2, 2 };
};

struct UpgradeState {
	int curVal = 0;
	int upgrade_amount = 0;
	int upgrade_cost = 0;
};

class StateManager
{
public:
	const int TILE_SIZE = 48;
	int map_height;
	int map_width;
	std::vector<std::vector<int>> map;
	uint map_index;
	uint saved_map_index;
	std::vector<std::string> map_lists;
	MapState current_map_state;

	// experience and upgrade
	int exp;
	UpgradeState health_upgrade;

	StateManager()
	{
		map_height = 3;
		map_width = 3;
		map = {
			{1, 2, 3},
			{4, 5, 6},
			{7, 8, 9}
		};
		map_index = 0;
		saved_map_index = 0;
		exp = 0;
		map_lists = { "tutorial", "map_1" };
		health_upgrade = { 0, 10, 1 };
	}

	MapState changeMap(std::string map_name);
	void save();
	void load();
};

extern StateManager state;