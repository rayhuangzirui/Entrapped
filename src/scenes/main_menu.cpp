#include "main_menu.hpp"
#include "tiny_ecs_registry.hpp"
#include <iostream>
#include "state_manager.hpp"
#include <world_init.hpp>

bool point_in_box(Motion motion, vec2 pos) {
	return pos.x > motion.position.x - motion.scale.x / 2
		&& pos.x < motion.position.x + motion.scale.x / 2
		&& pos.y > motion.position.y - motion.scale.y / 2
		&& pos.y < motion.position.y + motion.scale.y / 2;
}

void MainMenu::initialize(RenderSystem* renderer) {
	this->renderer = renderer;
	renderer->text_renderer.createText("Entrapped", { 50.f, window_height_px - 300.f }, 40.f, { 1.f, 1.f, 1.f });
	//renderer->text_renderer.createText("Press any key to start the game", { 40.f, window_height_px - 140.f }, 20.f, { 1.f, 1.f, 1.f });
	//renderer->text_renderer.createText("Press ; key to enter debug mode", { 40.f, window_height_px - 110.f }, 20.f, { 1.f, 1.f, 1.f });

	state.map_index = 0;
	new_game_button = createButton(renderer, { 50.f, window_height_px - 240.f }, { 200.f, 30.f }, "New Game");
	continue_game_button = createButton(renderer, { 50.f, window_height_px - 200.f }, { 200.f, 30.f }, "Continue Game");
	upgrade_button = createButton(renderer, { 50.f, window_height_px - 160.f }, { 200.f, 30.f }, "Upgrades");
	state.load();
	//exit_game_button = createButton({ 50.f, window_height_px - 120.f }, { 200.f, 30.f }, "Exit Game");
	//(vec2)mouse_position; // dummy to avoid compiler warning
}

void MainMenu::step(float elapsed_ms) {
	for (Entity button_entity : registry.pressedButtons.entities) {
		if (button_entity == new_game_button) {
			next_scene = "profession_menu";
			break;
		}
		else if (button_entity == continue_game_button) {
			state.map_index = state.saved_map_index;
			next_scene = "profession_menu";
			break;
		}
		else if (button_entity == upgrade_button) {
			next_scene = "upgrade_menu";
			break;
		}
	}
}

void MainMenu::destroy() {
	while (registry.motions.entities.size() > 0)
		registry.remove_all_components_of(registry.motions.entities.back());

}

void MainMenu::on_key(int key, int action, int mod) {
	(int)key;
	(int)action;
	(int)mod;

	//if (action == GLFW_RELEASE) {
	//	next_scene = "profession_menu";
	//	return;
	//}
}

std::string MainMenu::get_next_scene() {
	return this->next_scene;
}


void MainMenu::handle_collisions() {
	// dummy to avoid compiler warning
}

void MainMenu::on_mouse_move(vec2 mouse_position) {
	(vec2)mouse_position;
}

void MainMenu::on_mouse_click(int button, int action, int mod) {
	(RenderSystem*)renderer;
	(int)button;
	(int)action;
	(int)mod;
}

void MainMenu::draw_fps() {
	(RenderSystem*)renderer;
}
