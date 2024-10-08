// Header
#include "world_system.hpp"
#include "world_init.hpp"

// stlib
#include <cassert>
#include <sstream>

#include "physics_system.hpp"
#include <iostream>

// Game configuration
const size_t MAX_NUM_EELS = 15;
//const size_t MAX_NUM_FISH = 5;
const size_t ENEMY_SPAWN_DELAY_MS = 2000 * 3;
const size_t FISH_SPAWN_DELAY_MS = 5000 * 3;


// create the underwater world
WorldSystem::WorldSystem()
	//: points(0)
	//: next_eel_spawn(0.f)
	: next_enemy_spawn(0.f) {
	// Seeding rng with random device
	rng = std::default_random_engine(std::random_device()());
}

WorldSystem::~WorldSystem() {
	
	// destroy music components
	if (background_music != nullptr)
		Mix_FreeMusic(background_music);
	// replace with player dead sound
	if (player_dead_sound != nullptr)
		Mix_FreeChunk(player_dead_sound);
	if (enemy_dead_sound != nullptr)
		Mix_FreeChunk(enemy_dead_sound);
	if (enemy_hit_sound != nullptr)
		Mix_FreeChunk(enemy_hit_sound);
	if (bullet_hit_sound != nullptr)
		Mix_FreeChunk(bullet_hit_sound);
	if (bullet_fire_sound != nullptr)
		Mix_FreeChunk(bullet_fire_sound);


	Mix_CloseAudio();

	// Destroy all created components
	registry.clear_all_components();

	// Close the window
	glfwDestroyWindow(window);
}

// Debugging
namespace {
	void glfw_err_cb(int error, const char *desc) {
		fprintf(stderr, "%d: %s", error, desc);
	}
}

// World initialization
// Note, this has a lot of OpenGL specific things, could be moved to the renderer
GLFWwindow* WorldSystem::create_window() {
	///////////////////////////////////////
	// Initialize GLFW
	glfwSetErrorCallback(glfw_err_cb);
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW");
		return nullptr;
	}

	//-------------------------------------------------------------------------
	// If you are on Linux or Windows, you can change these 2 numbers to 4 and 3 and
	// enable the glDebugMessageCallback to have OpenGL catch your mistakes for you.
	// GLFW / OGL Initialization
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_RESIZABLE, 0);

	// Create the main window (for rendering, keyboard, and mouse input)
	window = glfwCreateWindow(window_width_px, window_height_px, "Salmon Game Assignment", nullptr, nullptr);
	if (window == nullptr) {
		fprintf(stderr, "Failed to glfwCreateWindow");
		return nullptr;
	}

	// Setting callbacks to member functions (that's why the redirect is needed)
	// Input is handled using GLFW, for more info see
	// http://www.glfw.org/docs/latest/input_guide.html
	glfwSetWindowUserPointer(window, this);
	auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2, _3); };
	auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_move({ _0, _1 }); };
	glfwSetKeyCallback(window, key_redirect);
	glfwSetCursorPosCallback(window, cursor_pos_redirect);

	//////////////////////////////////////
	// Loading music and sounds with SDL
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		fprintf(stderr, "Failed to initialize SDL Audio");
		return nullptr;
	}
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
		fprintf(stderr, "Failed to open audio device");
		return nullptr;
	}

	background_music = Mix_LoadMUS(audio_path("music.wav").c_str());
	player_dead_sound = Mix_LoadWAV(audio_path("death_sound.wav").c_str()); // replace with player dead sound
	//salmon_eat_sound = Mix_LoadWAV(audio_path("eat_sound.wav").c_str());
	
	// TODO: Sound effects are not added to the audio path yet
	enemy_dead_sound = Mix_LoadWAV(audio_path("enemy_dead.wav").c_str());
	enemy_hit_sound = Mix_LoadWAV(audio_path("enemy_hit.wav").c_str());
	bullet_hit_sound = Mix_LoadWAV(audio_path("bullet_hit.wav").c_str());
	bullet_fire_sound = Mix_LoadWAV(audio_path("bullet_fire.wav").c_str());

	if (background_music == nullptr || player_dead_sound == nullptr || enemy_dead_sound == nullptr
		|| enemy_hit_sound == nullptr || bullet_hit_sound == nullptr || bullet_fire_sound == nullptr) {
		fprintf(stderr, "Failed to load sounds\n %s\n %s\n %s\n make sure the data directory is present",
			audio_path("music.wav").c_str(),
			audio_path("death_sound.wav").c_str(),
			audio_path("enemy_dead.wav").c_str(),
			audio_path("enemy_hit.wav").c_str(),
			audio_path("bullet_hit.wav").c_str(),
			audio_path("bullet_fire.wav").c_str());
		return nullptr;
	}

	return window;
}

void WorldSystem::init(RenderSystem* renderer_arg) {
	this->renderer = renderer_arg;
	// Playing background music indefinitely
	Mix_PlayMusic(background_music, -1);
	fprintf(stderr, "Loaded music\n");

	// Set all states to default
    restart_game();
}

// Update our game world
bool WorldSystem::step(float elapsed_ms_since_last_update) {
	// Updating window title: Entrapped
	std::stringstream title_ss;
	title_ss << "Entrapped";
	glfwSetWindowTitle(window, title_ss.str().c_str());

	// Remove debug info from the last step
	while (registry.debugComponents.entities.size() > 0)
	    registry.remove_all_components_of(registry.debugComponents.entities.back());
	

	// spawn new enemies as A1 eels
	// TODO: change the enemy spawn logic
	next_enemy_spawn -= elapsed_ms_since_last_update * current_speed;
	if (registry.deadlys.components.size() <= MAX_NUM_EELS && next_enemy_spawn < 0.f) {
		// reset timer
		next_enemy_spawn = (ENEMY_SPAWN_DELAY_MS / 2) + uniform_dist(rng) * (ENEMY_SPAWN_DELAY_MS / 2);

		// create a new enemy at the right of the screen
		createEnemy(renderer, vec2(window_width_px + 30.f, 50.f + uniform_dist(rng) * (window_width_px - 100.f)));
	}

	// Processing the salmon state
	assert(registry.screenStates.components.size() <= 1);
    ScreenState &screen = registry.screenStates.components[0];

    float min_counter_ms = 3000.f;
	for (Entity entity : registry.deathTimers.entities) {
		// progress timer
		DeathTimer& counter = registry.deathTimers.get(entity);
		counter.counter_ms -= elapsed_ms_since_last_update;
		if(counter.counter_ms < min_counter_ms){
		    min_counter_ms = counter.counter_ms;
		}

		std::cout << "Death timer: " << counter.counter_ms << " ms" << std::endl;

		// restart the game once the death timer expired
		if (counter.counter_ms < 0) {
			registry.deathTimers.remove(entity);
			screen.darken_screen_factor = 0;
            restart_game();
			return true;
		}
	}
	// reduce window brightness if the player is dying
	screen.darken_screen_factor = 1 - min_counter_ms / 3000;

	// update LightUp timers and remove if time drops below zero, similar to the death counter
	for (Entity entity : registry.lightUps.entities) {
		// progress timer
		LightUp& counter = registry.lightUps.get(entity);
		counter.counter_ms -= elapsed_ms_since_last_update;

		std::cout << "LightUp timer: " << counter.counter_ms << " ms" << std::endl;

		if (counter.counter_ms < 0) {
			registry.lightUps.remove(entity); // remove the light up effect when timer ends
		}

	}
	return true;
}

// Reset the world state to its initial state
void WorldSystem::restart_game() {
	// Debugging for memory/component leaks
	registry.list_all_components();
	printf("Restarting\n");

	// Reset the game speed
	current_speed = 1.f;

	// Remove all entities that we created
	// All that have a motion, we could also iterate over all fish, eels, ... but that would be more cumbersome
	while (registry.motions.entities.size() > 0)
	    registry.remove_all_components_of(registry.motions.entities.back());

	// Debugging for memory/component leaks
	registry.list_all_components();

	// create a new Player entity
	
	// Player should be created at the beginning of the maze(currently set to the top-left corner of the screen)
	player_entity = createPlayer(renderer, {0, 0});

	registry.colors.insert(player_entity, { 1, 0.8f, 0.8f });
}

// Compute collisions between entities
void WorldSystem::handle_collisions() {
	// Loop over all collisions detected by the physics system
	auto& collisionsRegistry = registry.collisions;
	for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
		// The entity and its collider
		Entity entity = collisionsRegistry.entities[i];
		Entity entity_other = collisionsRegistry.components[i].other;

		// Player & Enemy collision: Enemy attacks the player, player loses 1 health, if health is 0, player dies
		if (registry.players.has(entity)) {
			if (registry.enemies.has(entity_other)) {

				// Player and enemy components
				Player& player = registry.players.get(entity);
				Enemy& enemy = registry.enemies.get(entity_other);

				// Reduce player health
				player.health -= enemy.damage;

				// Red tint light up effect on player
				registry.lightUps.emplace(entity); 
				//registry.colors.get(entity) = { 1.f, 0.f, 0.f }; // Red tint
				
				// TODO: change the light up color in the render system and shader

				// Play the enemy hit sound
				Mix_PlayChannel(-1, enemy_hit_sound, 0);

				// Check if the player is dead
				if (player.health <= 0) {
					// If the player is already dying, don't do anything
					if (registry.deathTimers.has(entity))
						continue;

					// initiate death unless already dying
					if (!registry.deathTimers.has(entity)) {
						// Scream, reset timer, and play the dead animation
						registry.deathTimers.emplace(entity);
						Mix_PlayChannel(-1, player_dead_sound, 0);

						// Player death animation
						Motion& player_entity_motion = registry.motions.get(entity);

						// Change the player's color, make it red on death
						registry.colors.get(entity) = { 1.f, 0.f, 0.f };
					}
					
				}
			}
		}

		// Bullet & Enemy collision: Bullet hits the enemy, enemy loses 1 health, if health is 0, enemy dies
		if (registry.bullets.has(entity)) {
			if (registry.enemies.has(entity_other)) {
				// Bullet and enemy components
				Bullet& bullet = registry.bullets.get(entity);
				Enemy& enemy = registry.enemies.get(entity_other);

				// Reduce enemy health
				enemy.health -= bullet.damage;

				// Red tint light up effect on enemy
				registry.lightUps.emplace(entity_other);
				//registry.colors.get(entity_other) = { 1.f, 0.f, 0.f }; // Red tint

				// Play the bullet hit sound
				Mix_PlayChannel(-1, bullet_hit_sound, 0);

				// Check if the enemy is dead
				if (enemy.health <= 0) {
					// TODO: play the enemy dead animation

					// Play the enemy dead sound
					Mix_PlayChannel(-1, enemy_dead_sound, 0);

					// Remove the enemy
					registry.remove_all_components_of(entity_other);
				}

				// Remove the bullet
				registry.remove_all_components_of(entity);
			}
		}
	}

	// Remove all collisions from this simulation step
	registry.collisions.clear();
}

// Add bullet creation
void WorldSystem::shoot_bullet() {
	// get the player's position
	Motion& player_motion = registry.motions.get(player_entity);

	// create a bullet moving in the direction the player is facing
	// calculated using the player's angle
	vec2 bullet_velocity = { cos(player_motion.angle) * 300.f, sin(player_motion.angle) * 300.f };

	// Create a bullet entity
	Entity bullet_entity = createBullet(renderer, player_motion.position, bullet_velocity);

	// player decrease 1 ammo
	Player& player = registry.players.get(player_entity);
	player.ammo--;

	// Play the bullet fire sound
	Mix_PlayChannel(-1, bullet_fire_sound, 0);
}

// Should the game be over ?
bool WorldSystem::is_over() const {
	return bool(glfwWindowShouldClose(window));
}

// On key callback
void WorldSystem::on_key(int key, int, int action, int mod) {
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A1: HANDLE SALMON MOVEMENT HERE
	// key is of 'type' GLFW_KEY_
	// action can be GLFW_PRESS GLFW_RELEASE GLFW_REPEAT
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// Handling salmon movement
	Motion& salmon_motion = registry.motions.get(player_entity);
	if (action == GLFW_PRESS || action == GLFW_RELEASE) {
		// Invalidate the directional keys if the salmon is dying
		// Setting the velocity to 0 if the key is released
		float velocity = (action == GLFW_PRESS) ? 200.f : 0.f;

		if (key == GLFW_KEY_UP) {
			if (registry.deathTimers.has(player_entity))
				return;
			salmon_motion.velocity.y = -velocity;
		}
		if (key == GLFW_KEY_DOWN) {
			if (registry.deathTimers.has(player_entity))
				return;
			salmon_motion.velocity.y = velocity;
		}

		if (key == GLFW_KEY_LEFT) {
			if (registry.deathTimers.has(player_entity))
				return;
			salmon_motion.velocity.x = cos(salmon_motion.angle) * velocity;
			salmon_motion.velocity.y = sin(salmon_motion.angle) * velocity;
		}
		if (key == GLFW_KEY_RIGHT) {
			if (registry.deathTimers.has(player_entity))
				return;
			salmon_motion.velocity.x = cos(salmon_motion.angle) * -velocity;
			salmon_motion.velocity.y = sin(salmon_motion.angle) * -velocity;
		}
	}

	// Resetting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_R) {
		int w, h;
		glfwGetWindowSize(window, &w, &h);

        restart_game();
	}

	// Debugging
	if (key == GLFW_KEY_D) {
		if (action == GLFW_RELEASE)
			debugging.in_debug_mode = false;
		else
			debugging.in_debug_mode = true;
	}

	// Control the current speed with `<` `>`
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_COMMA) {
		current_speed -= 0.1f;
		printf("Current speed = %f\n", current_speed);
	}
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_PERIOD) {
		current_speed += 0.1f;
		printf("Current speed = %f\n", current_speed);
	}
	current_speed = fmax(0.f, current_speed);

	// Handle closing the window by pressing ESC
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void WorldSystem::on_mouse_move(vec2 mouse_position) {
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A1: HANDLE SALMON ROTATION HERE
	// xpos and ypos are relative to the top-left of the window, the salmon's
	// default facing direction is (1, 0)
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	
	(vec2)mouse_position; // dummy to avoid compiler warning
}
