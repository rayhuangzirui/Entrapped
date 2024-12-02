#pragma once
# define M_PIl          3.141592653589793238462643383279502884L

#include "base_scene.hpp"
#include "render_system.hpp"
#include "camera_system.hpp"
#include <SDL.h>
#include <SDL_mixer.h>
#include <random>

class GameScene : public BaseScene {
public:
	void initialize(RenderSystem* renderer);
	void step(float elapsed_ms);
	void destroy();
	void on_key(int key, int action, int mod);
	std::string get_next_scene();
	void handle_collisions(float elapsed_ms_since_last_update);
	void on_mouse_move(vec2 mouse_position);
	void drawHealthBars();
	void on_mouse_click(int button, int action, int mod);
	vec2 mouse_position = { 0.f, 0.f };
	//vec2 fetchMousePosition();

private:
	RenderSystem* renderer;
    // Player state
	float current_speed;
	Entity player;
	Entity enemy;
	//TransState transState;
	//Entity transMaskEntity;
	std::string next_map_name;

	void createMaze();
	//bool check_player_wall_collision(const Motion& player_motion);
	bool check_aabb_collision(const vec2& box1_min, const vec2& box1_max, const vec2& box2_min, const vec2& box2_max);
	//Entity createHealthBar(RenderSystem* renderer, Entity entity, vec2 offset, vec2 size);
	Entity createHealthBarNew(Entity entity);
	Entity createPlayer(vec2 pos, std::string profession);
	Entity createGun(Entity player);
	Entity createPortal(vec2 pos, std::string map_name);
	Entity createEnemy(vec2 pos);
	Entity createEnemyAgile(vec2 pos);
	Entity createEnemyTank(vec2 pos);
	Entity createEnemyBoss(vec2 pos);


	Entity createWall(vec2 position, vec2 size);
	Entity createPlayerHPBar(vec2 position, float ratio);
	Entity createDirectionMarker(vec2 position);
	Entity createBackground();
	Entity createChest(vec2 position);
	Entity createPickup(vec2 position, PowerUpType type);

	Entity createTape(vec2 pos, int tape_num);
	void startTextAnimation(std::vector<std::string> text, int tape_num);
	void updateTextAnimation(float elapsed_ms);
	void renderAnimatedText(RenderSystem* renderer);
	Entity createTransitionMask(RenderSystem* renderer, float progress);

	void refreshUI(Entity player);
	void shoot_bullet(vec2 position, vec2 direction);

	void spawnEnemiesAndItems();

	void changeMap(std::string map_name, float elapsed_ms_since_last_update);

	void apply_damage(Entity& target, int damage);
	void show_damage_number(vec2 position, int damage);

	void updateHoverInfo();

	void restart_game();

	// player movement
	/*float PLAYER_SPEED = 200.0f;*/
	vec4 player_movement_state = { 0, 0, 0, 0 };

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
	Mix_Chunk* tape1_recording;
	Mix_Chunk* tape2_recording;
	Mix_Chunk* tape3_recording;
	Mix_Chunk* tape4_recording;
	Mix_Chunk* tape5_recording;
	Mix_Chunk* tape6_recording;
	Mix_Chunk* player_footstep;
	Mix_Chunk* enemy_footstep;
	Mix_Chunk* enemy_get_hit;
	Mix_Chunk* tape_pickup;

	vec2 player_velocity;

	std::string next_scene = "";
	CameraSystem camera;
	void updateCamera(const vec2& player_position);
	void updateCamera_smoothing(const vec2& player_position, const vec2& target_position);

	//void updateHints(Entity player);
	void updateHints(Entity player);

	//void renderCenteredText(const std::string& text, float screen_width, float screen_height, float scale, vec3 color);

	void createInventorySlots(Entity player);
	void refreshPowerUpUI(Entity player);
	void refreshInventorySlots(Entity player);

	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1

	bool is_left_mouse_pressed = false;
	float fire_timer = 0.f;  // Timer to control firing rate

	Entity hover_text_entity = Entity(); // Single hover text entity
};