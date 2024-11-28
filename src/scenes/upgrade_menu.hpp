#pragma once

#include "base_scene.hpp"
#include "render_system.hpp"

class UpgradeMenu : public BaseScene {
public:
	void initialize(RenderSystem* renderer);
	void step(float elapsed_ms);
	void destroy();
	void on_key(int key, int action, int mod);
	std::string get_next_scene();
	void handle_collisions(float elapsed_ms_since_last_update);
	void on_mouse_move(vec2 mouse_position);
	void on_mouse_click(int button, int action, int mod);
private:
	RenderSystem* renderer;
	std::string next_scene = "";

	void refreshUI();

	// Buttons
	Entity health_button;
	Entity ammo_button;
	Entity exit_button;
};