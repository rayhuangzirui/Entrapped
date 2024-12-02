#include "upgrade_menu.hpp"
#include "tiny_ecs_registry.hpp"
#include "state_manager.hpp"
#include <world_init.hpp>


void UpgradeMenu::initialize(RenderSystem* renderer) {
	this->renderer = renderer;

	health_button = createButton(renderer, { 350.f, 100.f }, { 105.f, 30.f }, "Upgrade");
	ammo_button = createButton(renderer, { 350.f, 150.f }, { 105.f, 30.f }, "Upgrade");

	exit_button = createButton(renderer, { 50.f, window_height_px-50.f }, { 105.f, 30.f }, "Finish");
	reset_button = createButton(renderer, { window_width_px - 150.f, 100.f }, { 105.f, 30.f }, "Reset");

	renderer->text_renderer.createText("*Upgrade will be applied on new games", { window_width_px - 450.f, window_height_px - 50.f }, 20.f, { 1.f, 1.f, 1.f });
	refreshUI();
	button_click = Mix_LoadWAV(audio_path("main-click.wav").c_str());
}

void UpgradeMenu::refreshUI() {
	while (registry.refreshables.entities.size() > 0)
		registry.remove_all_components_of(registry.refreshables.entities.back());

	Entity exp_text = renderer->text_renderer.createText("Experience: " + std::to_string(state.exp), { 50.f, 50.f }, 20.f, { 1.f, 1.f, 1.f });
	registry.refreshables.emplace(exp_text);

	Entity hp_text = renderer->text_renderer.createText("Extra Max HP: " + std::to_string(state.health_upgrade.curVal), { 50.f, 100.f }, 20.f, { 1.f, 1.f, 1.f });
	registry.refreshables.emplace(hp_text);
	Entity hp_pts = renderer->text_renderer.createText(std::to_string(state.health_upgrade.upgrade_cost) + " points", {455.f, 100.f}, 20.f, {1.f, 1.f, 1.f});
	registry.refreshables.emplace(hp_pts);

	Entity ammo_text = renderer->text_renderer.createText("Extra Initial Ammo: " + std::to_string(state.ammo_upgrade.curVal), { 50.f, 150.f }, 20.f, { 1.f, 1.f, 1.f });
	registry.refreshables.emplace(ammo_text);
	Entity ammo_pts = renderer->text_renderer.createText(std::to_string(state.ammo_upgrade.upgrade_cost) + " points", { 455.f, 150.f }, 20.f, { 1.f, 1.f, 1.f });
	registry.refreshables.emplace(ammo_pts);

}

void UpgradeMenu::step(float elapsed_ms) {
	refreshUI();
	for (Entity button_entity : registry.pressedButtons.entities) {
		Mix_PlayChannel(-1, button_click, 0);
		if (button_entity == health_button) {
			if (state.exp > state.health_upgrade.upgrade_cost) {
				state.health_upgrade.curVal += state.health_upgrade.upgrade_amount;
				state.exp -= state.health_upgrade.upgrade_cost;
			}
		}
		else if (button_entity == ammo_button) {
			if (state.exp > state.ammo_upgrade.upgrade_cost) {
				state.ammo_upgrade.curVal += state.ammo_upgrade.upgrade_amount;
				state.exp -= state.ammo_upgrade.upgrade_cost;
			}
		}else if (button_entity == exit_button) {
			next_scene = "main_menu";
			break;
		}
		else if (button_entity == reset_button) {
			state.exp = 0;
			state.ammo_upgrade.curVal = 0;
			state.health_upgrade.curVal = 0;
			state.save();
		}
	}
}

void UpgradeMenu::destroy() {
	while (registry.motions.entities.size() > 0)
		registry.remove_all_components_of(registry.motions.entities.back());

	state.save();
}

void UpgradeMenu::on_key(int key, int action, int mod) {
	(int)key;
	(int)action;
	(int)mod;
}

std::string UpgradeMenu::get_next_scene() {
	return this->next_scene;
}


void UpgradeMenu::handle_collisions(float elapsed_ms_since_last_update) {
	// dummy to avoid compiler warning
	(float)elapsed_ms_since_last_update;
}

void UpgradeMenu::on_mouse_move(vec2 mouse_position) {
	(vec2)mouse_position;
}

void UpgradeMenu::on_mouse_click(int button, int action, int mod) {
	(int)button;
	(int)action;
	(int)mod;
}

