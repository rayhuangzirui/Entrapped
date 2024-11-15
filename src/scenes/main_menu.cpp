#include "main_menu.hpp"
#include "tiny_ecs_registry.hpp"
#include <iostream>
#include "state_manager.hpp"

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
	new_game_button = createButton({ 50.f, window_height_px - 240.f }, { 200.f, 30.f }, "New Game");
	continue_game_button = createButton({ 50.f, window_height_px - 200.f }, { 200.f, 30.f }, "Continue Game");
	upgrade_button = createButton({ 50.f, window_height_px - 160.f }, { 200.f, 30.f }, "Upgrades");
	state.load();
	//exit_game_button = createButton({ 50.f, window_height_px - 120.f }, { 200.f, 30.f }, "Exit Game");
	//(vec2)mouse_position; // dummy to avoid compiler warning
}

void MainMenu::step(float elapsed_ms) {
	// pass
}

void MainMenu::destroy() {
	while (registry.motions.entities.size() > 0)
		registry.remove_all_components_of(registry.motions.entities.back());

}

void MainMenu::on_key(int key, int action, int mod) {
	(int)key;
	(int)action;
	(int)mod;

	if (action == GLFW_RELEASE) {
		next_scene = "profession_menu";
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

	for (Entity button_entity : registry.buttons.entities) {
		vec3& color = registry.colors.get(button_entity);
		Motion& motion = registry.motions.get(button_entity);
		if (point_in_box(motion, mouse_position)) {
			color = { 0.8f, 0.0f, 0.0f };
		}
		else {
			color = { 1.0f, 0.0f, 0.0f };
		}
	}
}

void MainMenu::on_mouse_click(int button, int action, int mod) {
	(RenderSystem*)renderer;
	(int)button;
	(int)action;
	(int)mod;

	if (action == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(renderer->window, &xpos, &ypos);
		vec2 mouse_position = { (float)xpos, (float)ypos };

		Motion& new_game_motion = registry.motions.get(new_game_button);
		Motion& continue_motion = registry.motions.get(continue_game_button);
		Motion& upgrade_motion = registry.motions.get(upgrade_button);
		if (point_in_box(new_game_motion, mouse_position)) {
			next_scene = "profession_menu";
		}
		else if (point_in_box(continue_motion, mouse_position)) {
			state.map_index = state.saved_map_index;
			next_scene = "profession_menu";
		}
		else if (point_in_box(upgrade_motion, mouse_position)) {
			next_scene = "upgrade_menu";
		}

		std::cout << mouse_position.x << " : " << mouse_position.y << std::endl;
	}
}

Entity MainMenu::createButton(vec2 pos, vec2 scale, std::string text) {
	Entity entity = Entity();

	// Create motion
	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	vec2 position = pos;
	position.x += (scale.x / 2.f - 15.f);
	motion.position = position;
	motion.scale = scale;

	registry.UIs.emplace(entity);
	registry.buttons.emplace(entity);

	registry.renderRequests.insert(
		entity, {
			TEXTURE_ASSET_ID::TEXTURE_COUNT,
			EFFECT_ASSET_ID::BOX,
			GEOMETRY_BUFFER_ID::DEBUG_LINE
		});

	registry.colors.insert(entity, { 1.0f, 0.0f, 0.0f });

	Entity text_entity = renderer->text_renderer.createText(text, pos, 20.f, { 1.f, 1.f, 1.f });

	return entity;
}

void MainMenu::draw_fps() {
	(RenderSystem*)renderer;
}
