#pragma once

#include "base_scene.hpp"
#include "render_system.hpp"
#include <SDL_mixer.h>

class MainMenu : public BaseScene {
public:
	void initialize(RenderSystem* renderer);
	void step(float elapsed_ms);
	void destroy();
	void on_key(int key, int action, int mod);
	std::string get_next_scene();
	void handle_collisions();
	void on_mouse_move(vec2 mouse_position);
	void on_mouse_click(int button, int action, int mod);
private:
	RenderSystem* renderer;
	std::string next_scene = "";
	
	// Buttons
	Entity new_game_button;
	Entity continue_game_button;
	Entity upgrade_button;
	Entity exit_game_button;
	Entity debug_button;

	Mix_Chunk* button_click;
};