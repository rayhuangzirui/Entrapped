#pragma once
#include <vector>
#include "common.hpp"

struct MapState {
	std::string name = "test";
	vec2 player_spawn = { 5, 5 };
	vec2 exit = { 2, 2 };
	int width = 0;
	int height = 0;
	std::vector<std::vector<int>> collision_layer;
	std::vector<std::vector<int>> decoration_layer;
	std::vector<std::vector<int>> interactive_layer;
};

MapState CreateTestMap();

MapState CreateTutorialMap_old();

MapState CreateTutorialMap();

MapState CreateMap1();

MapState CreateMap2();

