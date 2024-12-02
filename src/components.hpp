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

	float battery_level = 100.f;
	float max_battery_level = 100.f;
	float battery_timer = 3000.0f;

	// Hit cooldown
	float hit_cooldown = 0.f;

	// Dash cooldown
	float dash_cooldown = 500.f;

	// Player's initial ammo, associated with a weapon but currently associated with the player
	int ammo = 30;

	std::string profession;
	// if the player is a doctor, heal 1 health point every 5 seconds. Other professions have this timer set to 0.
	float heal_timer = 0; // ms

	// if the player is an hacker, increase 3 ammo per enemy killed. Other professions have this set to 0.
	float ammo_per_kill = 0;

	// Player's movement speed
	float speed = 200.0f; // Default speed
};

// Portal to the next map
struct Portal {
	std::string next_map = "test";
};

// Distinguish between text displayed in orthogonal and camera matrix
struct CameraText {

};


struct Button1 {
	Entity text;
};

// button that is pressed
struct PressedButton{

};


// Gun component
struct Gun
{
	// Gun's damage for each bullet hit
	int damage = 1;

	// Gun's fire rate in bullets per second
	float fire_rate = 5.f;

	// Gun's reload time in seconds
	//float reload_time = 0.5f;

	// Gun's ammo capacity
	int ammo_capacity = 30;
	int current_ammo = 30;
	//float bullet_speed = 500.f;

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

	int ricochet_count = 0;
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

	// Enemy type
	// 0 for normal enemy
	// 1 for agaile enemy
	// 2 for tank enemy
	// 3 for boss enemy
	int type = 0;

	Entity health_bar_entity;
};

// Enemy AI component
struct EnemyAI {
	vec2 last_player_position = { 0,0 };
	float detection_radius = 250;
	float chase_timer = 3000;
	float wander_timer = 0;
	float flee_timer = 3000;
	float path_finding_timer = 0;
	int state = 0;
	std::vector<vec2> path;
	float speed = 100.f; // movement speed of enemy
};

// Enemy Dash AI component
struct EnemyDashAI {
	float action_radius = 150;
	float dash_timer = 1000;
	float max_dash_timer = 1000;
	vec2 dash_direction = { 0, 0 };
};

// Boss AI component
struct BossAI {
	float action_radius = 100;
	float action_timer = 1000;
	float max_action_timer = 1000;
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
	float amount = 1;
};

struct AmmoChest {
	bool isOpen = false;
	float amount = 1;
};

struct RandomChest {
	bool isOpen = false;
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

// Structure to store collidable entities
struct Collidable {

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

struct DashCoolDown {
	float counter_ms = 1000;
};

// Background sprite that does not change with camera matrix
struct Background {

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

// Component that shows as a floating message
struct Message {

};

// UI component that requires refreshing
struct Refreshable {

};

// Text that displays FPS
struct FPSText {

};

// Tape component
struct Tape {
	int tape_num;
	bool is_played = false;
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

};

struct Hint {
	float radius;
	std::string text;
	bool is_visible = false;
	Entity text_entity;        // Stores the entity of the rendered text
};

//For draw order of FOV shader
struct FOV {
    
};

// Subtitle component
struct Subtitle {
	
};

// Hover over icons for text description
struct HoverInfo {
	std::string description; // Text to display
	Entity text_entity = Entity(); // Text entity for rendering
	vec2 offset = { 0.f, -20.f }; // Offset for positioning the text
	bool is_visible = false; // Whether the text is currently visible
	bool text_created = false;   // Flag to track if text entity has been created
};

//-------------------- Inventory system --------------------
// Inventory item definition
struct InventoryItem {
	enum class Type { None, AmmoPack, HealthPotion };
	Type type = Type::None;
	int count = 0;     // Number of items (if stackable)
	int max_count = 10; // Maximum stack size for stackable items
};

// Inventory structure with items and slots
struct Inventory {
	int max_slots = 4;
	std::vector<InventoryItem> items; // List of items in the inventory
};

// New component to tag entities as inventory slots
struct InventorySlot {
	int slot_index;  // The index of this slot in the inventory
};

// New component to tag entities as item icons (e.g., AmmoPack icon)
struct IconSprite {};

// New component to tag entities as item count text
struct InventoryItemCount {
	Entity text_entity;
};

// Dissovle transition component
struct TransState {
	bool is_fade_in = true;
	bool is_fade_out = false;
	float timer = 0.0f;
	float duration = 500.0f; // ms
};

struct TransMask {

};


//-------------------- Inventory system --------------------

//-------------------- Power up system --------------------
enum class PowerUpType {
	Shield,
	SpeedBoost,
	LifeSteal,
	RicochetPowerUp,
	AttackSpeedPowerUp,
	Soldier_init_powerup, 
	Doctor_init_powerup,
	Hacker_init_powerup,
};

// Component for PowerUp entities
struct PowerUp {
	PowerUpType type;  // Type of power-up
	int strength;      // Effect strength (e.g., shield points)
};

// Component for PowerUp pickups
struct Pickup
{
	PowerUpType item_type;
};


// Component for Shield effect on the player
struct Shield {
	int charges; // Number of shield charges
};

// Component for SpeedBoost effect on the player
struct SpeedBoost {
	int count; // Number of times speed has been increased
};

struct LifeSteal {
	int stacks; // Number of life steal stacks
};

struct RicochetPowerUp {
	int stacks;
};

struct AttackSpeedPowerUp {
	int stacks; // Number of times to increase fire_rate by 1.0f
};

struct PowerUpSlot {

};

struct Soldier_init_powerup {

};

struct Doctor_init_powerup {

};

struct Hacker_init_powerup {

};
//-------------------- Power up system --------------------

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

	MAN_WALK_1 = WOMAN_WALK_4 + 1,
	MAN_WALK_2 = MAN_WALK_1 + 1,
	MAN_WALK_3 = MAN_WALK_2 + 1,
	MAN_WALK_4 = MAN_WALK_3 + 1,

	BULLET_1 = MAN_WALK_4 + 1,
	BULLET_2 = BULLET_1 + 1,
	BULLET_3 = BULLET_2 + 1,

	PISTOL = BULLET_3 + 1,
	SMG = PISTOL + 1,
	RIFLE = SMG + 1,

	WOMAN_DEAD_1 = RIFLE + 1,
	WOMAN_DEAD_2 = WOMAN_DEAD_1 + 1,
	WOMAN_DEAD_3 = WOMAN_DEAD_2 + 1,

	MAN_DEAD_1 = WOMAN_DEAD_3+1,
	MAN_DEAD_2 = MAN_DEAD_1 + 1,
	MAN_DEAD_3 = MAN_DEAD_2 + 1,

	CHEST_CLOSED = MAN_DEAD_3 + 1,
	CHEST_OPENED = CHEST_CLOSED + 1,

	SPACE_BACKGROUND = CHEST_OPENED + 1,

	ITEM_SLOT = SPACE_BACKGROUND + 1,

	ITEM_AMMOPACK = ITEM_SLOT + 1,
	ITEM_MEDKIT = ITEM_AMMOPACK + 1,

	POWER_UP_SHIELD = ITEM_MEDKIT + 1,
	POWER_UP_SPEED_UP = POWER_UP_SHIELD + 1,
	POWER_UP_LIFE_STEAL = POWER_UP_SPEED_UP + 1,
	POWER_UP_RICOCHET = POWER_UP_LIFE_STEAL + 1,
	POWER_UP_ATTACK_SPEED = POWER_UP_RICOCHET + 1,

	CHOOSE_PROFESSION_TITLE = POWER_UP_ATTACK_SPEED + 1,
	SOLDIER_PAGE = CHOOSE_PROFESSION_TITLE + 1,
	SOLDIER_PAGE_CLICKED = SOLDIER_PAGE + 1,
	DOCTOR_PAGE = SOLDIER_PAGE_CLICKED + 1,
	DOCTOR_PAGE_CLICKED = DOCTOR_PAGE + 1,
	HACKER_PAGE = DOCTOR_PAGE_CLICKED + 1,
	HACKER_PAGE_CLICKED = HACKER_PAGE + 1,

	CONFIRM_BUTTON = HACKER_PAGE_CLICKED + 1,
	CONFIRM_BUTTON_CLICKED = CONFIRM_BUTTON + 1,
	CONFIRM_BUTTON_DISABLED = CONFIRM_BUTTON_CLICKED + 1,
	TAPE_1 = CONFIRM_BUTTON_DISABLED + 1,
	TAPE_2 = TAPE_1 + 1,
	TAPE_3 = TAPE_2 + 1,
	TAPE_4 = TAPE_3 + 1,
	TAPE_5 = TAPE_4 + 1,
	
	DOC_1 = TAPE_5 + 1,
	DOC_2 = DOC_1 + 1,
	DOC_3 = DOC_2 + 1,

	HACK_1 = DOC_3 + 1,
	HACK_2 = HACK_1 + 1,
	HACK_3 = HACK_2 + 1,

	SPIDER_WALK_1 = HACK_3+1,
	SPIDER_WALK_2 = SPIDER_WALK_1+1,
	SPIDER_WALK_3 = SPIDER_WALK_2+1,
	SPIDER_WALK_4 = SPIDER_WALK_3+1,

	SPIDER_DEAD_1 = SPIDER_WALK_4 + 1,
	SPIDER_DEAD_2 = SPIDER_DEAD_1 + 1,
	SPIDER_DEAD_3 = SPIDER_DEAD_2 + 1,
	SPIDER_DEAD_4 = SPIDER_DEAD_3 + 1,

	BOSS_WALK_1 = SPIDER_DEAD_4 + 1,
	BOSS_WALK_2 = BOSS_WALK_1 + 1,
	BOSS_WALK_3 = BOSS_WALK_2 + 1,
	BOSS_WALK_4 = BOSS_WALK_3 + 1,

	BOSS_DEAD_1 = BOSS_WALK_4+1,
	BOSS_DEAD_2 = BOSS_DEAD_1 + 1,
	BOSS_DEAD_3 = BOSS_DEAD_2 + 1,

	BOSS_ATTACK_1 = BOSS_DEAD_3 + 1,
	BOSS_ATTACK_2 = BOSS_ATTACK_1+1,
	BOSS_ATTACK_3 = BOSS_ATTACK_2 +1,
	BOSS_ATTACK_4 = BOSS_ATTACK_3 + 1,

	TEXTURE_COUNT = BOSS_ATTACK_4 + 1

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
	FOV2 = MAP + 1,
	FOV_NEW = FOV2+1,
	EFFECT_COUNT = FOV_NEW + 1,
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
	FOV_QUAD = SQUARE + 1,
	TANK_ENEMY = FOV_QUAD + 1,
	BOSS_ENEMY = TANK_ENEMY + 1,
	PLAYER_DOC = BOSS_ENEMY + 1,
	PLAYER_HACK = PLAYER_DOC + 1,
	GEOMETRY_COUNT = PLAYER_HACK + 1
};
const int geometry_count = (int)GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;

struct RenderRequest {
	TEXTURE_ASSET_ID used_texture = TEXTURE_ASSET_ID::TEXTURE_COUNT;
	// PLAYER_BACK_TEXTURE_ASSET_ID player_back = PLAYER_BACK_TEXTURE_ASSET_ID::TEXTURE_BACK_COUNT;
	EFFECT_ASSET_ID used_effect = EFFECT_ASSET_ID::EFFECT_COUNT;
	GEOMETRY_BUFFER_ID used_geometry = GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;
};