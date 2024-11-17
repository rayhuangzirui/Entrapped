#pragma once

#include "tiny_ecs_registry.hpp"
#include "common.hpp"
#include <render_system.hpp>

// deal with UI components that are independent 
class UISystem
{
public:
	void initialize(RenderSystem* renderer);
	void step(float elapsed_ms);
	void on_mouse_click(int button, int action, int mod);
	void on_mouse_move(vec2 mouse_position);
	void on_key(int key, int action, int mod);
	void draw_fps();
private:
	bool point_in_box(Motion motion, vec2 pos);
	RenderSystem* renderer;

	Entity FPS_entity;
};