#include "scene_system.hpp"
#include <iostream>
#include "main_menu.hpp"
#include "game_scene.hpp"
#include "death_scene.hpp"
#include "tiny_ecs_registry.hpp"

SceneSystem::SceneSystem() {
	this->renderer = nullptr;
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

void SceneSystem::step(float elapsed_ms) {
	current_scene->step(elapsed_ms);
	std::string next_scene = current_scene->get_next_scene();
	if (next_scene.compare("") != 0) {
		changeScene(next_scene);
		//std::cout << "err here" << std::endl;
		pushScene();
	}
}

void SceneSystem::popScene() {
	current_scene->destroy();
}

void SceneSystem::on_key(int key, int action, int mod) {
	current_scene->on_key(key, action, mod);

	//std::string next_scene = current_scene->get_next_scene();
	//if (next_scene.compare("") != 0) {
	//	changeScene(next_scene);
	//	//std::cout << "err here" << std::endl;
	//	pushScene();
	//}
}

void SceneSystem::handle_collisions() {
	current_scene->handle_collisions();
}

void SceneSystem::on_mouse_move(vec2 mouse_position) {
	current_scene->on_mouse_move(mouse_position);
}

void SceneSystem::on_mouse_click(int button, int action, int mod) {
	current_scene->on_mouse_click(button, action, mod);
}

void SceneSystem::draw_fps() {
	current_scene->draw_fps();
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
	else if (name.compare("death_scene") == 0) {
		this->popScene();
		delete this->current_scene;
		this->current_scene = new DeathScene();
	}
	else {
		std::cout << "Error: Unknow Scene Name" << std::endl;
	}
}