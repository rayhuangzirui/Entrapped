#pragma once

#include "scene_system.hpp"
#include "render_system.hpp"

class BaseScene {
public: 
	virtual ~BaseScene() = default;
	virtual void initialize(RenderSystem* renderer) = 0;
	virtual void step(RenderSystem* renderer) = 0;
	virtual void destroy(RenderSystem* renderer) = 0;

};