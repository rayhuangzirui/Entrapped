#pragma once
#include <vector>
#include "common.hpp"

struct MapState {
	std::string map_name = "test";
	vec2 player_spawn = { 5, 5 };
	vec2 exit = { 2, 2 };
	std::vector<std::vector<int>> colission_layer;
	std::vector<std::vector<int>> decoration_layer;
	std::vector<std::vector<int>> interactive_layer;
};

MapState CreateTestMap();

bool GetTextureID(int id);
bool is_blocked(int id);
bool is_floor(int id);
bool is_wall(int id);



