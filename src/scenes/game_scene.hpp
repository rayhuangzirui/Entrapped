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
	void render_maze(RenderSystem* renderer);     //maze renderer

	bool check_aabb_collision(const vec2& box1_min, const vec2& box1_max, const vec2& box2_min, const vec2& box2_max);
	bool check_player_wall_collision(const Motion& player_motion);
	Entity createPlayer(RenderSystem* renderer, vec2 pos);
	Entity createEnemy(RenderSystem* renderer, vec2 pos);


	float PLAYER_SPEED = 100.0f;
	const int TILE_SIZE = 48;

};