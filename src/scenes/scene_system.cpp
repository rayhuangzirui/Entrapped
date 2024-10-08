#include "scene_system.hpp"
#include <iostream>

SceneSystem::SceneSystem() {
	current_scene = createMainMenuScene();
	std::cout << ">>> Scene System Initialized" << std::endl;
}

Entity createMainMenuScene() {
	auto entity = Entity();

	return entity;
}

Entity createGameScene() {
	auto entity = Entity();

	return entity;
}