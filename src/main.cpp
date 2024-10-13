
#define GL3W_IMPLEMENTATION
#include <gl3w.h>

// stlib
#include <chrono>

// internal
#include "physics_system.hpp"
#include "render_system.hpp"
#include "world_system.hpp"
#include <iostream>
#include <scenes/scene_system.hpp>
#include <text_renderer.hpp>
#include <ai_system.hpp>

using Clock = std::chrono::high_resolution_clock;

// Entry point
int main()
{
	std::cout << "Main method entered" << std::endl;
	// Global systems
	WorldSystem world;
	RenderSystem renderer;
	PhysicsSystem physics;
	//BaseScene* game_scene = new GameScene();
	AISystem ai_system;


	// Initializing window
	GLFWwindow* window = world.create_window();
	if (!window) {
		// Time to read the error message
		printf("Press any key to exit");
		getchar();
		return EXIT_FAILURE;
	}
	// initialize the main systems
	renderer.init(window);
	world.init(&renderer);
	//TextRenderer text_renderer;
	//text_renderer.loadFont();

	// variable timestep loop
	auto t = Clock::now();
	while (!world.is_over()) {
		// Processes system messages, if this wasn't present the window would become unresponsive
		glfwPollEvents();

		// Calculating elapsed times in milliseconds from the previous iteration
		auto now = Clock::now();
		float elapsed_ms =
			(float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000;
		t = now;
		world.step(elapsed_ms);
		ai_system.step(elapsed_ms);
		physics.step(elapsed_ms);
		//game_scene->handle_collisions();

		renderer.draw();
	}

	return EXIT_SUCCESS;
}
