#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "base_scene.hpp"
#include "main_menu.hpp"
#include <render_system.hpp>

class SceneSystem
{
public: 
	SceneSystem();
	~SceneSystem();
	BaseScene* current_scene;
	void initialize(RenderSystem* renderer);
	void pushScene();
	void step();
	void popScene();
private:
	RenderSystem* renderer;
};