#pragma once

#include "base_scene.hpp"
#include "render_system.hpp"
#include <SDL.h>
#include <SDL_mixer.h>

class GameScene : public BaseScene {
public:
	void initialize(RenderSystem* renderer);
	void step(RenderSystem* renderer);
	void destroy(RenderSystem* renderer);
	void on_key(RenderSystem* renderer, int key, int action, int mod);
	std::string get_next_scene();
	void handle_collisions();


private:
    // Player state
	float current_speed;
	Entity player;
	Entity enemy;
	void render_maze(RenderSystem* renderer);
	bool check_player_wall_collision(const Motion& player_motion);
	bool check_aabb_collision(const vec2& box1_min, const vec2& box1_max, const vec2& box2_min, const vec2& box2_max);
	Entity createPlayer(RenderSystem* renderer, vec2 pos);
	Entity createEnemy(RenderSystem* renderer, vec2 pos);
	Entity createWall(RenderSystem* renderer, vec2 position, vec2 size);

	float PLAYER_SPEED = 100.0f;
	const int TILE_SIZE = 48;
	Mix_Music* background_music;
	Mix_Chunk* player_dead_sound;
	vec2 player_velocity;
	int music_volume = 10;  // Default max volume
	std::string next_scene = "";

};