#include "ui_system.hpp"

void UISystem::step(float elapsed_ms_since_last_update)
{
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

void UISystem::on_mouse_click(RenderSystem* renderer, int button, int action, int mod) {
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

bool UISystem::point_in_box(Motion motion, vec2 pos) {
	return pos.x > motion.position.x - motion.scale.x / 2
		&& pos.x < motion.position.x + motion.scale.x / 2
		&& pos.y > motion.position.y - motion.scale.y / 2
		&& pos.y < motion.position.y + motion.scale.y / 2;
}