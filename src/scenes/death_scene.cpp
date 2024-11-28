#include "death_scene.hpp"
#include "tiny_ecs_registry.hpp"


void DeathScene::initialize(RenderSystem* renderer) {
	this->renderer = renderer;
	renderer->text_renderer.createText("You are dead", { window_width_px/2-120.f, window_height_px/2-20.f }, 40.f, { 1.f, 1.f, 1.f });
	renderer->text_renderer.createText("Press Q to return to the main menu", { window_width_px / 2 - 160.f, window_height_px/2+40.f }, 20.f, { 1.f, 1.f, 1.f });
	//renderer->text_renderer.createText("Main Menu", { window_width_px / 2 + 40.f, window_height_px/2+40.f }, 20.f, { 1.f, 1.f, 1.f });
	//(vec2)mouse_position; // dummy to avoid compiler warning
}

void DeathScene::step(float elapsed_ms) {
	// pass
}

void DeathScene::destroy() {
	while (registry.motions.entities.size() > 0)
		registry.remove_all_components_of(registry.motions.entities.back());
}

void DeathScene::on_key(int key, int action, int mod) {
	(int)key;
	(int)action;
	(int)mod;
	if (action == GLFW_RELEASE && key == GLFW_KEY_Q) {
		next_scene = "main_menu";
		return;
	}
}

std::string DeathScene::get_next_scene() {
	return this->next_scene;
}


void DeathScene::handle_collisions(float elapsed_ms_since_last_update) {
	// dummy to avoid compiler warning
	(float)elapsed_ms_since_last_update;
}

void DeathScene::on_mouse_move(vec2 mouse_position) {
	(vec2)mouse_position;
}

void DeathScene::on_mouse_click(int button, int action, int mod) {
	(int)button;
	(int)action;
	(int)mod;
}


