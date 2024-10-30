#include "maze_generator.hpp"
#include <stack>
#include <cstdlib>
#include <ctime>
#include <algorithm>


std::vector<Direction> directions = {
    {0, -1},  // Up
    {0, 1},   // Down
    {-1, 0},  // Left
    {1, 0}    // Right
};

// this is to ensure the directions are shuffled and not in the same order, basically randomizing the directions
void shuffle(std::vector<Direction>& dir) {
    for (int i = dir.size() - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        std::swap(dir[i], dir[j]);
    }
}

// backtrack genetation of the maze
void generateMaze(int maze[MAZE_HEIGHT][MAZE_WIDTH]) {
    
    for (int y = 0; y < MAZE_HEIGHT; ++y) {
        for (int x = 0; x < MAZE_WIDTH; ++x) {
            maze[y][x] = Cell::WALL;
        }
    }

    // Start from (1, 1)
    std::stack<std::pair<int, int>> stack;
    stack.push({1, 1});
    maze[1][1] = Cell::EMPTY;

    while (!stack.empty()) {
        auto top = stack.top();
        int cx = top.first;
        int cy = top.second;
        shuffle(directions);  // Shuffle movement directions

        bool moved = false;
        for (const auto& dir : directions) {
            int nx = cx + dir.dx * 2;
            int ny = cy + dir.dy * 2;

            if (nx >= 1 && nx < MAZE_WIDTH - 1 && ny >= 1 && ny < MAZE_HEIGHT - 1 &&
                maze[ny][nx] == Cell::WALL) {
                
                // Knock down the wall between cells
                maze[cy + dir.dy][cx + dir.dx] = Cell::EMPTY;
                maze[ny][nx] = Cell::EMPTY;

                stack.push({nx, ny});
                moved = true;
                break;
            }
        }

        if (!moved) {
            stack.pop();  
        }
    }
}