#pragma once

#include "base_scene.hpp"
#include "render_system.hpp"

class MainMenu : public BaseScene {
public:
	void initialize(RenderSystem* renderer);
	void step(RenderSystem* renderer);
	void destroy(RenderSystem* renderer);
};