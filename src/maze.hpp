#pragma once

#define MAZE_HEIGHT 100
#define MAZE_WIDTH 100

#define BOX_MAZE_HEIGHT 15
#define BOX_MAZE_WIDTH 26

#include <vector>

extern const int box_testing_environment[BOX_MAZE_HEIGHT][BOX_MAZE_WIDTH];
extern const int maze[MAZE_HEIGHT][MAZE_WIDTH];
extern const int tutorial_maze[MAZE_HEIGHT][MAZE_WIDTH];
extern std::vector<std::vector<int>> box_testing_environtment_vector;
extern std::vector<std::vector<int>> tutorial_maze_vector;