#pragma once

#define MAZE_HEIGHT 100
#define MAZE_WIDTH 100

#include <vector>

struct Map {
    std::vector<std::vector<int>> colission_layer;
    std::vector<std::vector<int>> item_layer;
    std::vector<std::vector<int>> enemy_layer;
    std::vector<std::vector<int>> decoration_layer;
};
