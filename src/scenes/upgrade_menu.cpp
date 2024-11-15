#include "upgrade_menu.hpp"
#include "tiny_ecs_registry.hpp"
#include "state_manager.hpp"


void UpgradeMenu::initialize(RenderSystem* renderer) {
	this->renderer = renderer;
	renderer->text_renderer.createText("Experience: " + std::to_string(state.exp), { 50.f, 50 }, 20.f, { 1.f, 1.f, 1.f });
	//(vec2)mouse_position; // dummy to avoid compiler warning
}

void UpgradeMenu::step(float elapsed_ms) {
	// pass
}

void UpgradeMenu::destroy() {
	while (registry.motions.entities.size() > 0)
		registry.remove_all_components_of(registry.motions.entities.back());
}

void UpgradeMenu::on_key(int key, int action, int mod) {
	(int)key;
	(int)action;
	(int)mod;
	if (action == GLFW_RELEASE && key == GLFW_KEY_Q) {
		next_scene = "main_menu";
		return;
	}
}

std::string UpgradeMenu::get_next_scene() {
	return this->next_scene;
}


void UpgradeMenu::handle_collisions() {
	// dummy to avoid compiler warning
}

void UpgradeMenu::on_mouse_move(vec2 mouse_position) {
	(vec2)mouse_position;
}

void UpgradeMenu::on_mouse_click(int button, int action, int mod) {
	(int)button;
	(int)action;
	(int)mod;
}

void UpgradeMenu::draw_fps() {
	// pass
}

