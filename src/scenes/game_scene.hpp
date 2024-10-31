#pragma once

#include "base_scene.hpp"
#include "render_system.hpp"
#include "camera_system.hpp"
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
	void on_mouse_move(vec2 mouse_position);
	void on_mouse_click(RenderSystem* renderer, int button, int action, int mod);
	void draw_fps(RenderSystem* renderer);

private:
    // Player state
	float current_speed;
	Entity player;
	Entity enemy;
	void render_maze(RenderSystem* renderer);
	bool check_player_wall_collision(const Motion& player_motion);
	bool check_aabb_collision(const vec2& box1_min, const vec2& box1_max, const vec2& box2_min, const vec2& box2_max);
	Entity createPlayer(RenderSystem* renderer, vec2 pos);
	Entity createGun(RenderSystem* renderer, Entity player);
	Entity createEnemy(RenderSystem* renderer, vec2 pos);
	Entity createWall(RenderSystem* renderer, vec2 position, vec2 size);
	void shoot_bullet(RenderSystem* renderer, vec2 position, vec2 direction);

	void apply_damage(Entity& target, int damage);
	void show_damage_number(RenderSystem* renderer, vec2 position, int damage);
	void update_gun_position(Entity player, Entity gun);

	float PLAYER_SPEED = 100.0f;
	const int TILE_SIZE = 48;
	Mix_Music* background_music;
	Mix_Chunk* player_dead_sound;
	vec2 player_velocity;

	std::string next_scene = "";
	CameraSystem camera;
	void updateCamera(const vec2& player_position);
};