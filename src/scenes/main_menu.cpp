#include "main_menu.hpp"
#include "tiny_ecs_registry.hpp"


void MainMenu::initialize(RenderSystem* renderer) {
	renderer->text_renderer.createText("Entrapped", { 50.f, window_height_px - 200.f }, 40.f, { 1.f, 1.f, 1.f });
	renderer->text_renderer.createText("Press any key to start the game", { 40.f, window_height_px - 140.f }, 20.f, { 1.f, 1.f, 1.f });
	renderer->text_renderer.createText("Press ; key to enter debug mode", { 40.f, window_height_px - 110.f }, 20.f, { 1.f, 1.f, 1.f });
	//(vec2)mouse_position; // dummy to avoid compiler warning
}

void MainMenu::step(RenderSystem* renderer) {
	(RenderSystem*)renderer;
}

void MainMenu::destroy(RenderSystem* renderer) {
	while (registry.motions.entities.size() > 0)
		registry.remove_all_components_of(registry.motions.entities.back());
	(RenderSystem*)renderer;
}

void MainMenu::on_key(RenderSystem* renderer, int key, int action, int mod) {
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

	if (action == GLFW_RELEASE) {
		next_scene = "game_scene";
		return;
	}
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

void MainMenu::on_mouse_click(RenderSystem* renderer, int button, int action, int mod) {
	(RenderSystem*)renderer;
	(int)button;
	(int)action;
	(int)mod;
}

void MainMenu::draw_fps(RenderSystem* renderer) {
	(RenderSystem*)renderer;
}
