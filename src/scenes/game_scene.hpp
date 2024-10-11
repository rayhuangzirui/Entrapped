#pragma once

#include "base_scene.hpp"
#include "render_system.hpp"

class GameScene : public BaseScene {
public:
	void initialize(RenderSystem* renderer);
	void step(RenderSystem* renderer);
	void destroy(RenderSystem* renderer);
	void on_key(RenderSystem* renderer, int key, int action, int mod);
	void handle_collisions();
	
private:
    // Player state
	float current_speed;
	Entity player;
	Entity enemy;
	Entity createPlayer(RenderSystem* renderer, vec2 pos);
	Entity createEnemy(RenderSystem* renderer, vec2 pos);
	float PLAYER_SPEED = 100.0f;
};