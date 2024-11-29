#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "base_scene.hpp"
#include "main_menu.hpp"
#include "game_scene.hpp"
#include "death_scene.hpp"
#include <render_system.hpp>

class SceneSystem
{
public: 
	SceneSystem();
	~SceneSystem();
	BaseScene* current_scene;
	std::string profession;
	void initialize(RenderSystem* renderer);
	void pushScene();
	void step(float elapsed_ms);
	void on_key(int key, int action, int mod);
	void popScene();
	void changeScene(std::string name);
	void handle_collisions(float elapsed_ms_since_last_update);
	void on_mouse_move(vec2 mouse_position);
	void on_mouse_click(int button, int action, int mod);
	void before_exit();
private:
	RenderSystem* renderer;
};