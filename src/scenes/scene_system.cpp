#include "scene_system.hpp"
#include <iostream>
#include "main_menu.hpp"
#include "game_scene.hpp"

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

void SceneSystem::on_key(int key, int action, int mod) {
	current_scene->on_key(this->renderer, key, action, mod);

	std::string next_scene = current_scene->get_next_scene();
	if (next_scene.compare("") != 0) {
		changeScene(next_scene);
		//std::cout << "err here" << std::endl;
		pushScene();
	}
}

void SceneSystem::handle_collisions() {
	current_scene->handle_collisions();
}

void SceneSystem::on_mouse_move(vec2 mouse_position) {
	current_scene->on_mouse_move(mouse_position);
}

void SceneSystem::on_mouse_click(int button, int action, int mod) {
	current_scene->on_mouse_click(this->renderer, button, action, mod);
}

void SceneSystem::changeScene(std::string name) {
	if (name.compare("main_menu") == 0) {
		this->popScene();
		delete this->current_scene;
		this->current_scene = new MainMenu();
	}
	else if (name.compare("game_scene") == 0) {
		this->popScene();
		delete this->current_scene;
		this->current_scene = new GameScene();
	}
	else {
		std::cout << "Error: Unknow Scene Name" << std::endl;
	}
}