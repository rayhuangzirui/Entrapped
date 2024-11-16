#pragma once

// internal
#include "common.hpp"

// stlib
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

#include "render_system.hpp"
#include <scenes/scene_system.hpp>
#include <ui_system.hpp>

// Container for all our entities and game logic. Individual rendering / update is
// deferred to the relative update() methods
class WorldSystem
{
public:
	WorldSystem();

	// Creates a window
	GLFWwindow* create_window();

	// starts the game
	void init(RenderSystem* renderer);

	// Releases all associated resources
	~WorldSystem();

	// Steps the game ahead by ms milliseconds
	bool step(float elapsed_ms);

	// Should the game be over ?
	bool is_over()const;

private:
	// Input callback functions
	void on_key(int key, int, int action, int mod);
	void on_mouse_move(vec2 pos);
	void on_mouse_click(int button, int action, int mod);
	SceneSystem scene_system;
	UISystem ui_system;

	// OpenGL window handle
	GLFWwindow* window;

	// Game state
	RenderSystem* renderer;

	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1
};
