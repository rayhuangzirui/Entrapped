#include "profession_menu.hpp"
#include "tiny_ecs_registry.hpp"
#include <iostream>
#include "state_manager.hpp"
#include "world_init.hpp"


void ProfessionMenu::initialize(RenderSystem* renderer) {
	this->renderer = renderer;
	create_profession();

	state.map_index = 0;
}

void ProfessionMenu::step(float elapsed_ms) {
	// remove all text entities
	while (registry.texts.entities.size() > 0)
		registry.remove_all_components_of(registry.texts.entities.back());

	while (registry.backgrounds.entities.size() > 0)
		registry.remove_all_components_of(registry.backgrounds.entities.back());

	if (!selection_delay_complete && selected_profession != "") {
		time_since_selection += elapsed_ms;
		if (time_since_selection >= 200.f) {
			time_since_selection = 0.f; 
			selection_delay_complete = true;
		}
	}

	if (confirm_button_clicked) {
		time_since_last_click += elapsed_ms;
		if (time_since_last_click >= 500.f) {
			confirm_button_clicked = false;
			transition_complete = true;
		}
	}
 
	createBackground(renderer);
	// Title of the profession menu, positioned at the top of the screen
	//renderer->text_renderer.createText("Choose Your Profession", { window_width_px / 2 - 250.f, 100.f }, 40.f, { 1.f, 1.f, 1.f });
	createChooseProfessionTitle(renderer);

	for (auto& profession : professions) {
		createProfessionLayer(renderer, profession.position, "Soldier", profession.is_selected);
	}

	// Create a confirm button
	if (selection_delay_complete) {
		createConfirmButton(renderer, { window_width_px / 2, window_height_px - 100 }, confirm_button_enabled, confirm_button_clicked);
	}
	else {
		createConfirmButton(renderer, { window_width_px / 2, window_height_px - 100 }, false, false);
	}

	if (transition_complete) {
		next_scene = "game_scene";
	}
}

void ProfessionMenu::destroy() {
	while (registry.motions.entities.size() > 0)
		registry.remove_all_components_of(registry.motions.entities.back());

}

void ProfessionMenu::on_key(int key, int action, int mod) {
	(int)key;
	(int)action;
	(int)mod;

	// Press ; to enter debug mode before initializing player, enemies, and bullets
	// in order to visualize the meshes by rendering .obj rather than sprites 
	if (key == GLFW_KEY_SEMICOLON) {
		if (action == GLFW_PRESS)
			debugging.in_debug_mode = false;
		else
			debugging.in_debug_mode = true;
	}
}

std::string ProfessionMenu::get_next_scene() {
	return this->next_scene;
}


void ProfessionMenu::handle_collisions() {
	// dummy to avoid compiler warning
}

void ProfessionMenu::on_mouse_move(vec2 mouse_position) {
	(vec2)mouse_position;
}

void ProfessionMenu::on_mouse_click(int button, int action, int mod) {
	(RenderSystem*)renderer;
	(int)button;
	(int)action;
	(int)mod;

	if (action == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(renderer->window, &xpos, &ypos);
		vec2 mouse_position = { (float)xpos, (float)ypos };

		for (auto& profession : professions) {
			if (mouse_position.x > profession.position.x - profession.size.x / 2 - 10.f && mouse_position.x < profession.position.x + profession.size.x/2 - 10.f &&
				mouse_position.y > profession.position.y - profession.size.y / 2 - 15.f && mouse_position.y < profession.position.y + profession.size.y/2 - 16.f) {
				on_profession_selected(profession.name);
				time_since_selection = 0.f;
				selection_delay_complete = false;
			}
		}
		
		// Confirm button
		if (mouse_position.x > window_width_px / 2 - 75.f && mouse_position.x < window_width_px / 2 + 58.f &&
			mouse_position.y > window_height_px - 100 - 25.f && mouse_position.y < window_height_px - 100 + 33.f) {
			confirm_button_clicked = true;
			printf("Confirm button clicked\n");
			time_since_last_click = 0.f;
		}
	}
}

void ProfessionMenu::draw_fps() {
	(RenderSystem*)renderer;
}

void ProfessionMenu::create_profession() {
	Profession soldier;
	soldier.name = "Soldier";
	soldier.description = "";
	soldier.position = { window_width_px / 3 - 200, window_height_px / 2 + 20 };
	soldier.size = { 353.f, 414.f }; //642, 753
	soldier.is_selected = false;
	professions.push_back(soldier);

	Profession doctor;
	doctor.name = "Doctor";
	doctor.description = "";
	doctor.position = { 640.f, window_height_px/2 + 20 };
	doctor.size = { 353.f, 414.f };
	doctor.is_selected = false;
	professions.push_back(doctor);

	Profession hacker;
	hacker.name = "Hacker";
	hacker.description = "";
	hacker.position = { 1066.f, window_height_px/2 +20 };
	hacker.size = { 353.f, 414.f };
	hacker.is_selected = false;
	professions.push_back(hacker);
}


void ProfessionMenu::on_profession_selected(const std::string& profession) {
	for (auto& prof : professions) {
		if (prof.name == profession) {
			prof.is_selected = true;
			selected_profession = prof.name;
			confirm_button_enabled = true;
		}
		else {
			prof.is_selected = false;
		}
	}
}

