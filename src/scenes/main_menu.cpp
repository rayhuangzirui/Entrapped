#include "main_menu.hpp"
#include "tiny_ecs_registry.hpp"


void MainMenu::initialize(RenderSystem* renderer) {
	renderer->text_renderer.createText("Main Menu", { 50.f, window_height_px - 200.f }, 40.f, { 1.f, 1.f, 1.f });
	renderer->text_renderer.createText("New Game", { 50.f, window_height_px - 140.f }, 20.f, { 1.f, 1.f, 1.f });
	renderer->text_renderer.createText("Upgrades", { 50.f, window_height_px - 110.f }, 20.f, { 1.f, 1.f, 1.f });
	renderer->text_renderer.createText("Quit", { 50.f, window_height_px - 80.f }, 20.f, { 1.f, 1.f, 1.f });
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
}


void MainMenu::handle_collisions() {
	// dummy to avoid compiler warning
}

