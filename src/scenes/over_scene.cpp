#include "over_scene.hpp"
#include "tiny_ecs_registry.hpp"


void OverScene::initialize(RenderSystem* renderer) {
	this->renderer = renderer;
	renderer->text_renderer.createText("Thank you for playing the demo!", { window_width_px / 2 - 300.f, window_height_px / 2 - 20.f }, 40.f, { 1.f, 1.f, 1.f });
	renderer->text_renderer.createText("Press Q to return to the main menu", { window_width_px / 2 - 160.f, window_height_px / 2 + 40.f }, 20.f, { 1.f, 1.f, 1.f });
	//renderer->text_renderer.createText("Main Menu", { window_width_px / 2 + 40.f, window_height_px/2+40.f }, 20.f, { 1.f, 1.f, 1.f });
	//(vec2)mouse_position; // dummy to avoid compiler warning
}

void OverScene::step(float elapsed_ms) {
	// pass
}

void OverScene::destroy() {
	while (registry.motions.entities.size() > 0)
		registry.remove_all_components_of(registry.motions.entities.back());
}

void OverScene::on_key(int key, int action, int mod) {
	(int)key;
	(int)action;
	(int)mod;
	if (action == GLFW_RELEASE && key == GLFW_KEY_Q) {
		next_scene = "main_menu";
		return;
	}
}

std::string OverScene::get_next_scene() {
	return this->next_scene;
}


void OverScene::handle_collisions() {
	// dummy to avoid compiler warning
}

void OverScene::on_mouse_move(vec2 mouse_position) {
	(vec2)mouse_position;
}

void OverScene::on_mouse_click(int button, int action, int mod) {
	(int)button;
	(int)action;
	(int)mod;
}


