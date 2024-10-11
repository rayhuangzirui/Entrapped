#pragma once

#include "render_system.hpp"

class BaseScene {
public: 
	virtual ~BaseScene() = default;
	virtual void initialize(RenderSystem* renderer) = 0;
	virtual void step(RenderSystem* renderer) = 0;
	virtual void destroy(RenderSystem* renderer) = 0;
	virtual void on_key(RenderSystem* renderer, int key, int action, int mod) = 0;
	virtual void handle_collisions() = 0;
};