#pragma once
# define M_PIl          3.141592653589793238462643383279502884L

#include "base_scene.hpp"
#include "render_system.hpp"
#include "camera_system.hpp"
#include <SDL.h>
#include <SDL_mixer.h>

class GameScene : public BaseScene {
public:
	void initialize(RenderSystem* renderer);
	void step(float elapsed_ms);
	void destroy();
	void on_key(int key, int action, int mod);
	std::string get_next_scene();
	void handle_collisions();
	void on_mouse_move(vec2 mouse_position);
	void drawHealthBars(RenderSystem* renderer);
	void on_mouse_click(int button, int action, int mod);
	void draw_fps();

private:
	RenderSystem* renderer;
    // Player state
	float current_speed;
	Entity player;
	Entity enemy;
	void createMaze();
	bool check_player_wall_collision(const Motion& player_motion);
	bool check_aabb_collision(const vec2& box1_min, const vec2& box1_max, const vec2& box2_min, const vec2& box2_max);
	Entity createHealthBar(RenderSystem* renderer, Entity entity, vec2 offset, vec2 size);
	Entity createChest(RenderSystem* renderer, vec2 pos);
	Entity createPlayer(vec2 pos);
	Entity createGun(Entity player);
	Entity createPortal(vec2 pos, std::string map_name);
	Entity createEnemy(vec2 pos);
	Entity createWall(vec2 position, vec2 size);
	Entity createPlayerHPBar(vec2 position, float ratio);
	Entity createHealthChest(vec2 position);
	void refreshUI(Entity player);
	void shoot_bullet(vec2 position, vec2 direction);

	void spawnEnemiesAndItems();

	void changeMap(std::string map_name);

	void apply_damage(Entity& target, int damage);
	void show_damage_number(vec2 position, int damage);

	void restart_game();

	float PLAYER_SPEED = 200.0f;
	const int TILE_SIZE = 48;
	Mix_Music* background_music;
	Mix_Chunk* player_dead_sound;
	Mix_Chunk* player_hurt_sound;
	Mix_Chunk* monster_dead_sound;
	Mix_Chunk* monster_hurt_sound;
	Mix_Chunk* explode_sound;
	Mix_Chunk* shoot_sound;
	Mix_Chunk* health_pickup_sound;
	Mix_Chunk* item_pickup_sound;
	Mix_Chunk* reload_sound;
	Mix_Chunk* stab_sound;
	vec2 player_velocity;

	std::string next_scene = "";
	CameraSystem camera;
	void updateCamera(const vec2& player_position);
	void GameScene::updateCamera_smoothing(const vec2& player_position, const vec2& target_position);

	Entity FPS_entity;
};