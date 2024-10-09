#include "scene_system.hpp"
#include <iostream>
#include "main_menu.cpp"

SceneSystem::SceneSystem() {
	this->current_scene = new MainMenu();
	std::cout << ">>> Scene System Initialized" << std::endl;
}

SceneSystem::~SceneSystem() {
	delete this->current_scene;
}

void SceneSystem::initialize(RenderSystem* renderer) {
	this->renderer = renderer;
}

void SceneSystem::pushScene() {
	current_scene->initialize(this->renderer);
}

void SceneSystem::step() {
	current_scene->step(this->renderer);
}

void SceneSystem::popScene() {
	current_scene->destroy(this->renderer);
}