#pragma once

#include "render_system.hpp"


class BaseScene {
public: 
	virtual ~BaseScene() = default;
	virtual void initialize(RenderSystem* renderer) = 0;
	virtual void step(float elapsed_ms) = 0;
	virtual void destroy() = 0;
	virtual void on_key(int key, int action, int mod) = 0;
	virtual std::string get_next_scene() = 0;
	virtual void handle_collisions() = 0;
	virtual void on_mouse_move(vec2 mouse_position) = 0;
	virtual void on_mouse_click(int button, int action, int mod) = 0;
	std::string selected_profession;
private:
	RenderSystem* renderer;
	std::string next_scene = "";
};