
#pragma once
#include <vector>

// const int MAZE_WIDTH = 100;
// const int MAZE_HEIGHT = 100;

#define MAZE_WIDTH 100
#define MAZE_HEIGHT 100

enum Cell {
    WALL = 1,
    EMPTY = 0
};

// Struct for maze generation directions
struct Direction {
    int dx, dy;
};

// Declare the functions
void generateMaze(int maze[MAZE_HEIGHT][MAZE_WIDTH]);
