#pragma once

#include "render_system.hpp"


class BaseScene {
public: 
	virtual ~BaseScene() = default;
	virtual void initialize(RenderSystem* renderer) = 0;
	virtual void step(RenderSystem* renderer) = 0;
	virtual void destroy(RenderSystem* renderer) = 0;
	virtual void on_key(RenderSystem* renderer, int key, int action, int mod) = 0;
	virtual std::string get_next_scene() = 0;
	virtual void handle_collisions() = 0;
	virtual void on_mouse_move(vec2 mouse_position) = 0;
	virtual void on_mouse_click(RenderSystem* renderer, int button, int action, int mod) = 0;
	virtual void draw_fps(RenderSystem* renderer) = 0;
private:
	std::string next_scene = "";
};