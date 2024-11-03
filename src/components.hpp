#pragma once
#include "common.hpp"
#include <vector>
#include <unordered_map>
#include "../ext/stb_image/stb_image.h"
#include <string>

// Parent-Child relationship
struct Parent
{
	Entity parent; 
};


// Player component
struct Player
{
	// Player's health
	int health = 20;

	int max_health = 20;

	// Hit cooldown
	float hit_cooldown = 0.f;

	// Player's initial ammo, associated with a weapon but currently associated with the player
	int ammo = 30;

	// Player's Profession may be used in the future
	//enum Profession { SOLDIER, DOCTOR, HACKER } profession;
};

// Portal to the next map
struct Portal {
	std::string next_map = "test";
};

// Distinguish between text displayed in orthogonal and camera matrix
struct CameraText {

};



// Gun component
struct Gun
{
	// Gun's damage for each bullet hit
	int damage = 1;

	// Gun's fire rate in bullets per second
	float fire_rate = 0.5f;

	// Gun's reload time in seconds
	float reload_time = 0.5f;

	// Gun's ammo capacity
	int ammo_capacity = 30;
	int current_ammo = 30;
	float bullet_speed = 500.f;

	// Gun's bullet direction
	vec2 direction = { 0, 0 };

	// Gun's bullet damage
	int bullet_damage = 1;

	// Gun's rotation angle
	float angle = 0.f;

	// Position offset of the gun from the player
	vec2 offset = { 20.f, -6.f };
	//vec2 offset = { 0, 0 };
};

// Bullet component
struct Bullet
{
	// Bullet's damage
	int damage = 1;

	// Bullet's speed
	float speed = 500.f;
	vec2 direction = { 0, 0 };

};

// FPS counter component
struct FPS
{
	bool visible = false;
	float fps = 0.f;
	float elapsed_time = 0.f;
	int frame_count = 0;
};

// Enemy component
struct Enemy
{
	// Enemy's health
	int health = 2;
	
	// Enemy's damage
	int damage = 1;

	Entity health_bar_entity;
};

// Enemy AI component
struct EnemyAI {
	vec2 last_player_position = { 0,0 };
	float detection_radius = 250;
	float chase_timer = 3000;
	float wander_timer = 0;
	float flee_timer = 3000;
	int state = 0;
};

struct Health {
	int current_health;  // Health points of an entity
	int max_health; 
};

struct DashTimer {
	float counter_ms;  // Duration of dash in milliseconds
};

// placeholder entities that are invisible
struct Invisibles {

};

// marker to mark a location on map;
struct DirectionMarker {
	vec2 exit_position = { 0, 0 };
};

struct Chest
{
	bool isOpen = false;
};

struct HealthChest {
	bool isOpen = false;
	float amount = 5;
};

struct AmmoChest {
	bool isOpen = false;
	float amount = 10;
};

// component used to render text
struct Text {
	std::string content;
};

// text for overlay HUD
struct HUD {

};

// All data relevant to the shape and motion of entities
struct Motion {
	vec2 position = { 0, 0 };
	float angle = 0;
	vec2 velocity = { 0, 0 };
	vec2 scale = { 10, 10 };
};

// Health bar component for enemies
struct HealthBar {
	Entity owner;  // Entity that the health bar is associated with
	
};


// Stucture to store collision information
struct Collision
{
	// Note, the first object is stored in the ECS container.entities
	Entity other; // the second object involved in the collision
	Collision(Entity& other) { this->other = other; };
};

struct DamageText {
	float duration_ms;  // Time left for the damage text to stay on screen
	vec2 position;      // Position of the damage text
};

// Data structure for toggling debug mode
struct Debug {
	bool in_debug_mode = 0;
	bool in_freeze_mode = 0;
};
extern Debug debugging;

// Sets the brightness of the screen
struct ScreenState
{
	float darken_screen_factor = -1;
};

// A struct to refer to debugging graphics in the ECS
struct DebugComponent
{
	// Note, an empty struct has size 1
};

// A timer that will be associated to dying salmon
struct DeathTimer
{
	float counter_ms = 3000;
};

struct EnemyDeathTime
{
	float initial_ms = 3000;
	float counter_ms = 3000;
};

// Added LightUp component: A timer that will be associated with the player lighting up after getting attacked by an enemy
struct LightUp
{
	float counter_ms = 500; // light up for 0.5 second
	float blink_timer = 0.0f; // timer for blinking effect
	bool active = false; // flag to indicate if the light up effect is active
};

// AI timer component
struct AITimer
{
	float interval = 2000.0f; // 2 second, period of time before the enemy changes direction
	float counter_ms = 0.0f; // 1 second
};

// A timer that will be associated with the damage cooldown of the player
struct DamageCoolDown
{
	float counter_ms = 500; // damage cooldown for 0.5 second
};

// Opacity component
struct Opacity
{
	float opacity = 1.0f;
};

// A timer that will be associated with the knockback of the enemy
struct KnockbackTimer
{
	float counter_ms = 100.f; // knockback for 0.1 second
};

// Component to distinguish between UI and game elements
struct UI {

};

// Text that displays FPS
struct FPSText {

};

// Component for the Player hp bar
struct PlayerHPBar {
	float max_length = 100.f;
};

// Single Vertex Buffer element for non-textured meshes (coloured.vs.glsl & salmon.vs.glsl)
struct ColoredVertex
{
	vec3 position;
	vec3 color;
};

// Single Vertex Buffer element for textured sprites (textured.vs.glsl)
struct TexturedVertex
{
	vec3 position;
	vec2 texcoord;
};

// Mesh datastructure for storing vertex and index buffers
struct Mesh
{
	static bool loadFromOBJFile(std::string obj_path, std::vector<ColoredVertex>& out_vertices, std::vector<uint16_t>& out_vertex_indices, vec2& out_size);
	vec2 original_size = {1,1};
	std::vector<ColoredVertex> vertices;
	std::vector<uint16_t> vertex_indices;
};

// Bounding box component used for collision detection
struct BoundingBox {
	vec2 min; // Minimum x, y coordinates of the bounding box
	vec2 max; // Maximum x, y coordinates of the bounding box
};

// The map
struct Map {
	int width;
	int height;
	std::vector<std::vector<int>> grid;
};

struct Hint {
	float radius;
	std::string text;
	bool is_visible = false;
	Entity text_entity;        // Stores the entity of the rendered text
};


/**
 * The following enumerators represent global identifiers refering to graphic
 * assets. For example TEXTURE_ASSET_ID are the identifiers of each texture
 * currently supported by the system.
 *
 * So, instead of referring to a game asset directly, the game logic just
 * uses these enumerators and the RenderRequest struct to inform the renderer
 * how to structure the next draw command.
 *
 * There are 2 reasons for this:
 *
 * First, game assets such as textures and meshes are large and should not be
 * copied around as this wastes memory and runtime. Thus separating the data
 * from its representation makes the system faster.
 *
 * Second, it is good practice to decouple the game logic from the render logic.
 * Imagine, for example, changing from OpenGL to Vulkan, if the game logic
 * depends on OpenGL semantics it will be much harder to do the switch than if
 * the renderer encapsulates all asset data and the game logic is agnostic to it.
 *
 * The final value in each enumeration is both a way to keep track of how many
 * enums there are, and as a default value to represent uninitialized fields.
 */


enum class TEXTURE_ASSET_ID {
	PLAYER_1 = 0,
	PLAYER_2 = PLAYER_1 + 1,
	PLAYER_3 = PLAYER_2 + 1,

	/*PLAYER_BACK_1 = PLAYER_3 + 1,
	PLAYER_BACK_2 = PLAYER_BACK_1 + 1,
	PLAYER_BACK_3 = PLAYER_BACK_2 + 1,

	PLAYER_FRONT_1 = PLAYER_BACK_3 + 1,
	PLAYER_FRONT_2 = PLAYER_FRONT_1 + 1,
	PLAYER_FRONT_3 = PLAYER_FRONT_2 + 1,*/

	DOOR_OPEN = PLAYER_3 + 1,
	DOOR_CLOSED = DOOR_OPEN + 1,

	WALL_1 = DOOR_CLOSED + 1,
	WALL_2 = WALL_1 + 1,
	WALL_3 = WALL_2 + 1,
	WALL_4 = WALL_3 + 1,
	WALL_5 = WALL_4 + 1,
	WALL_6 = WALL_5 + 1,

	FLOOR_1 = WALL_6 + 1,
	FLOOR_2 = FLOOR_1 + 1,
	FLOOR_3 = FLOOR_2 + 1,
	FLOOR_4 = FLOOR_3 + 1,
	FLOOR_5 = FLOOR_4 + 1,

	WOMAN_WALK_1 = FLOOR_5 + 1,
	WOMAN_WALK_2 = WOMAN_WALK_1 + 1,
	WOMAN_WALK_3 = WOMAN_WALK_2 + 1,
	WOMAN_WALK_4 = WOMAN_WALK_3 + 1,

	BULLET_1 = WOMAN_WALK_4 + 1,
	BULLET_2 = BULLET_1 + 1,
	BULLET_3 = BULLET_2 + 1,

	PISTOL = BULLET_3 + 1,
	SMG = PISTOL + 1,
	RIFLE = SMG + 1,

	WOMAN_DEAD_1 = RIFLE + 1,
	WOMAN_DEAD_2 = WOMAN_DEAD_1 + 1,
	WOMAN_DEAD_3 = WOMAN_DEAD_2 + 1,

	CHEST_CLOSED = WOMAN_DEAD_3 + 1,
	CHEST_OPENED = CHEST_CLOSED + 1,

	TEXTURE_COUNT = CHEST_OPENED + 1
};
const int texture_count = (int)TEXTURE_ASSET_ID::TEXTURE_COUNT;

enum class EFFECT_ASSET_ID {
	COLOURED = 0,
	DEBUG_LINE = COLOURED + 1,
	MESHED = DEBUG_LINE + 1,
	TEXTURED = MESHED + 1,
	TEXT = TEXTURED + 1,
	RING = TEXT + 1,
	RECTANGLE = RING + 1,
	BOX = RECTANGLE + 1,
	GLOBAL = BOX+1,
	MAP = GLOBAL + 1,
	EFFECT_COUNT = MAP + 1,
};
const int effect_count = (int)EFFECT_ASSET_ID::EFFECT_COUNT;

enum class GEOMETRY_BUFFER_ID {
	MESH = 0,
	SPRITE = MESH + 1,
	DEBUG_LINE = SPRITE + 1,
	SCREEN_TRIANGLE = DEBUG_LINE + 1,
	TEXT = SCREEN_TRIANGLE + 1,
	PLAYER = TEXT + 1,
	BULLET = PLAYER + 1,
	ENEMY_WOMAN = BULLET + 1,
	MAZE = ENEMY_WOMAN  + 1,
	SQUARE = MAZE + 1,
	GEOMETRY_COUNT = SQUARE + 1

};
const int geometry_count = (int)GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;

struct RenderRequest {
	TEXTURE_ASSET_ID used_texture = TEXTURE_ASSET_ID::TEXTURE_COUNT;
	// PLAYER_BACK_TEXTURE_ASSET_ID player_back = PLAYER_BACK_TEXTURE_ASSET_ID::TEXTURE_BACK_COUNT;
	EFFECT_ASSET_ID used_effect = EFFECT_ASSET_ID::EFFECT_COUNT;
	GEOMETRY_BUFFER_ID used_geometry = GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;
};