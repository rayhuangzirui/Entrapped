#include "ui_system.hpp"

void UISystem::initialize(RenderSystem* renderer) {
	this->renderer = renderer;

	// fps entity
	FPS_entity = Entity();
	registry.fps.emplace(FPS_entity);
}

void UISystem::step(float elapsed_ms_since_last_update)
{	
	registry.pressedButtons.clear();

	if (registry.fps.size() <= 0) {
		registry.fps.emplace(FPS_entity);
	}

	FPS& fps_counter = registry.fps.get(FPS_entity);

	fps_counter.elapsed_time += elapsed_ms_since_last_update;
	fps_counter.frame_count++;

	if (fps_counter.elapsed_time >= 1000) {
		fps_counter.fps = fps_counter.frame_count / (fps_counter.elapsed_time / 1000.f);
		fps_counter.frame_count = 0;
		fps_counter.elapsed_time = 0.0;
	}

	draw_fps();
	(float)elapsed_ms_since_last_update;
}

void UISystem::on_mouse_move(vec2 mouse_position) {

	// hover effect
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

void UISystem::on_mouse_click(int button, int action, int mod) {
	if (action == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(renderer->window, &xpos, &ypos);
		vec2 mouse_position = { (float)xpos, (float)ypos };

		// send pressed events
		registry.pressedButtons.clear();
		for (Entity button_entity : registry.buttons.entities) {
			Motion& motion = registry.motions.get(button_entity);
			if (point_in_box(motion, mouse_position)) {
				registry.pressedButtons.emplace(button_entity);
			}
		}
	}
}

void UISystem::on_key(int key, int action, int mod) {
	if (action == GLFW_PRESS && key == GLFW_KEY_F) {
		// get the fps entity
		Entity fps_entity = FPS_entity;
		FPS& fps_counter = registry.fps.get(fps_entity);
		// visialize fps
		fps_counter.visible = !fps_counter.visible;
		printf("FPS counter visibility: %d\n", fps_counter.visible);
		printf("FPS: %f\n", fps_counter.fps);
	}
}

bool UISystem::point_in_box(Motion motion, vec2 pos) {
	return pos.x > motion.position.x - motion.scale.x / 2
		&& pos.x < motion.position.x + motion.scale.x / 2
		&& pos.y > motion.position.y - motion.scale.y / 2
		&& pos.y < motion.position.y + motion.scale.y / 2;
}

void UISystem::draw_fps() {
	if (registry.fpsTexts.entities.size() > 0) {
		registry.remove_all_components_of(registry.fpsTexts.entities.back());
	}
	Entity fps_entity = registry.fps.entities[0];
	FPS& fps_counter = registry.fps.get(fps_entity);
	if (fps_counter.visible) {
		std::string fps_string = "FPS: " + std::to_string(static_cast<int> (fps_counter.fps));

		// Update the text content
		//fps_text.content = fps_string;
		vec2 fps_position = vec2(10.f, 10.f);
		Entity text = renderer->text_renderer.createText(fps_string, fps_position, 20.f, { 0.f, 1.f, 0.f });
		registry.fpsTexts.emplace(text);
		//registry.cameraTexts.emplace(text);
	}
}