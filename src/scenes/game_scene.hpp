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
	void render_maze(RenderSystem* renderer);

	Entity createPlayer(RenderSystem* renderer, vec2 pos);
	Entity createEnemy(RenderSystem* renderer, vec2 pos);

	bool check_collision(const BoundingBox& box1, const BoundingBox& box2);
	bool is_player_colliding_with_wall();

	float PLAYER_SPEED = 100.0f;
	const int TILE_SIZE = 48;

};