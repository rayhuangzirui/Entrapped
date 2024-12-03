#pragma once

#include "base_scene.hpp"
#include "render_system.hpp"
#include <SDL_mixer.h>

struct Profession {
	std::string name;
	std::string description;
	vec2 position;
	vec2 size;

	bool is_selected;
};

class ProfessionMenu : public BaseScene {
public:
	void initialize(RenderSystem* renderer);
	void step(float elapsed_ms);
	void destroy();
	void on_key(int key, int action, int mod);
	std::string get_next_scene();
	void handle_collisions(float elapsed_ms_since_last_update);
	void on_mouse_move(vec2 mouse_position);
	void on_mouse_click(int button, int action, int mod);
	std::string selected_profession;
	TransState transState;
private:
	RenderSystem* renderer;
	std::string next_scene = "";
	std::vector<Profession> professions;

	bool confirm_button_enabled = false;
	bool confirm_button_clicked = false;

	// time for delay and animation control
	float time_since_last_click = 0.f;
	bool transition_complete = false;
	float time_since_selection = 0.f;
	bool selection_delay_complete = false;

	void create_profession();
	void on_profession_selected(const std::string& profession);

	Mix_Chunk* button_click;
};