#include "death_scene.hpp"
#include "tiny_ecs_registry.hpp"


void DeathScene::initialize(RenderSystem* renderer) {
	renderer->text_renderer.createText("You are dead", { window_width_px/2-120.f, window_height_px/2-20.f }, 40.f, { 1.f, 1.f, 1.f });
	renderer->text_renderer.createText("Restart", { window_width_px / 2 - 120.f, window_height_px/2+40.f }, 20.f, { 1.f, 1.f, 1.f });
	renderer->text_renderer.createText("Main Menu", { window_width_px / 2 + 40.f, window_height_px/2+40.f }, 20.f, { 1.f, 1.f, 1.f });
	//(vec2)mouse_position; // dummy to avoid compiler warning
}

void DeathScene::step(RenderSystem* renderer) {
	(RenderSystem*)renderer;
}

void DeathScene::destroy(RenderSystem* renderer) {
	while (registry.motions.entities.size() > 0)
		registry.remove_all_components_of(registry.motions.entities.back());
	(RenderSystem*)renderer;
}

void DeathScene::on_key(RenderSystem* renderer, int key, int action, int mod) {
	(int)key;
	(int)action;
	(int)mod;
}

std::string DeathScene::get_next_scene() {
	return this->next_scene;
}


void DeathScene::handle_collisions() {
	// dummy to avoid compiler warning
}

