#pragma once

#include "tiny_ecs_registry.hpp"
#include "common.hpp"
#include <render_system.hpp>

// deal with UI components that are independent 
class UISystem
{
public:
	void step(float elapsed_ms);
	void on_mouse_click(RenderSystem* renderer, int button, int action, int mod);
	void on_mouse_move(vec2 mouse_position);
private:
	bool point_in_box(Motion motion, vec2 pos);
};