#include "upgrade_menu.hpp"
#include "tiny_ecs_registry.hpp"
#include "state_manager.hpp"
#include <world_init.hpp>


void UpgradeMenu::initialize(RenderSystem* renderer) {
	this->renderer = renderer;
	renderer->text_renderer.createText("Experience: " + std::to_string(state.exp), { 50.f, 50.f }, 20.f, { 1.f, 1.f, 1.f });

	health_button = createButton(renderer, { 350.f, 100.f }, { 105.f, 30.f }, "Upgrade");
	refreshUI();
}

void UpgradeMenu::refreshUI() {
	while (registry.refreshables.entities.size() > 0)
		registry.remove_all_components_of(registry.refreshables.entities.back());

	Entity hp_text = renderer->text_renderer.createText("Extra Max HP: " + std::to_string(state.health_upgrade.curVal), { 50.f, 100.f }, 20.f, { 1.f, 1.f, 1.f });
	registry.refreshables.emplace(hp_text);
	
	Entity hp_pts = renderer->text_renderer.createText(std::to_string(state.health_upgrade.upgrade_amount) + " points", {455.f, 100.f}, 20.f, {1.f, 1.f, 1.f});
	registry.refreshables.emplace(hp_pts);
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
	//for (Entity button_entity : registry.buttons.entities) {
	//	vec3& color = registry.colors.get(button_entity);
	//	Motion& motion = registry.motions.get(button_entity);
	//	if (point_in_box(motion, mouse_position)) {
	//		color = { 0.8f, 0.0f, 0.0f };
	//	}
	//	else {
	//		color = { 1.0f, 0.0f, 0.0f };
	//	}
	//}
}

void UpgradeMenu::on_mouse_click(int button, int action, int mod) {
	(int)button;
	(int)action;
	(int)mod;
}

void UpgradeMenu::draw_fps() {
	// pass
}

