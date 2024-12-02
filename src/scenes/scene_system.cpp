#include "scene_system.hpp"
#include <iostream>
#include "main_menu.hpp"
#include "game_scene.hpp"
#include "death_scene.hpp"
#include "over_scene.hpp"
#include "profession_menu.hpp"
#include "upgrade_menu.hpp"
#include "tiny_ecs_registry.hpp"

SceneSystem::SceneSystem() {
	this->renderer = nullptr;
	this->current_scene = new MainMenu();
	std::cout << ">>> Scene System Initialized" << std::endl;
}

SceneSystem::~SceneSystem() {
	//delete this->current_scene;
}

void SceneSystem::before_exit() {
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

void SceneSystem::handle_collisions(float elapsed_ms_since_last_update) {
	current_scene->handle_collisions(elapsed_ms_since_last_update);
}

void SceneSystem::on_mouse_move(vec2 mouse_position) {
	current_scene->on_mouse_move(mouse_position);
}

void SceneSystem::on_mouse_click(int button, int action, int mod) {
	current_scene->on_mouse_click(button, action, mod);
}

void SceneSystem::changeScene(std::string name) {
	if (name.compare("main_menu") == 0) {
		this->popScene();
		delete this->current_scene;
		this->current_scene = new MainMenu();
	}
	else if (name.compare("game_scene") == 0) {
		this->popScene();
		this->profession = static_cast<ProfessionMenu*>(this->current_scene)->selected_profession;
		delete this->current_scene;
		this->current_scene = new GameScene();
		this->current_scene->selected_profession = this->profession;
	}
	else if (name.compare("death_scene") == 0) {
		this->popScene();
		delete this->current_scene;
		this->current_scene = new DeathScene();
	}
	else if (name.compare("over_scene") == 0) {
		this->popScene();
		delete this->current_scene;
		this->current_scene = new OverScene();
	}
	else if (name.compare("profession_menu") == 0) {
		this->popScene();
		delete this->current_scene;
		this->current_scene = new ProfessionMenu();
	}
	else if (name.compare("upgrade_menu") == 0) {
		this->popScene();
		delete this->current_scene;
		this->current_scene = new UpgradeMenu();
	}
	else {
		std::cout << "Error: Unknow Scene Name" << std::endl;
	}
}