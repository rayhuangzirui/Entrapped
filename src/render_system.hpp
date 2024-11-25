#pragma once

#include <array>
#include <utility>

#include "common.hpp"
#include "components.hpp"
#include "camera_system.hpp"
#include "tiny_ecs.hpp"
#include <text_renderer.hpp>



// System responsible for setting up OpenGL and for rendering all the
// visual entities in the game
class RenderSystem {
	CameraSystem camera_system;

	/**
	 * The following arrays store the assets the game will use. They are loaded
	 * at initialization and are assumed to not be modified by the render loop.
	 *
	 * Whenever possible, add to these lists instead of creating dynamic state
	 * it is easier to debug and faster to execute for the computer.
	 */
	std::array<GLuint, texture_count> texture_gl_handles;
	std::array<ivec2, texture_count> texture_dimensions;

	// Make sure these paths remain in sync with the associated enumerators.
	// Associated id with .obj path
	const std::vector < std::pair<GEOMETRY_BUFFER_ID, std::string>> mesh_paths =
	{
		//std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::PLAYER, mesh_path("player_mesh.obj")),
		std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::PLAYER, mesh_path("player_min_v.obj")),
		std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::BULLET, mesh_path("bullet_min_v.obj")),
		std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::ENEMY_WOMAN, mesh_path("woman_min_v.obj")),
		// specify meshes of other assets here
	};

	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, texture_count> texture_paths = {
			textures_path("player_1_new.png"),
			textures_path("player_3_new.png"),
			textures_path("player_2_new.png"),

			textures_path("tile_door_open.png"),
			textures_path("tile_door_closed.png"),

			textures_path("tile_wall_1.png"),
			textures_path("tile_wall_2.png"),
			textures_path("tile_wall_3.png"),
			textures_path("tile_wall_4.png"),
			textures_path("tile_wall_5.png"),
			textures_path("tile_wall_6.png"),

			textures_path("tile_floor_1.png"),
			textures_path("tile_floor_2.png"),
			textures_path("tile_floor_3.png"),
			textures_path("tile_floor_4.png"),
			textures_path("tile_floor_5.png"),

			textures_path("woman_walk_1.png"),
			textures_path("woman_walk_2.png"),
			textures_path("woman_walk_3.png"),
			textures_path("woman_walk_4.png"),

			textures_path("bullet_1.png"),
			textures_path("bullet_2.png"),
			textures_path("bullet_3.png"),

			textures_path("pistol.png"),
			textures_path("SMG.png"),
			textures_path("rifle.png"),

			textures_path("woman_hurt_dead_1.png"),
			textures_path("woman_hurt_dead_2.png"),
			textures_path("woman_hurt_dead_3.png"),

			textures_path("chest_closed.png"),
			textures_path("chest_opened.png"),

			textures_path("Space Background.png"),

			textures_path("item_slot.png"),
			textures_path("item_ammopack.png"),
			textures_path("item_medkit.png"),
			
			textures_path("power_up_shield.png"),
			textures_path("power_up_speedup.png"),

			textures_path("choose_profession_title.png"),
			textures_path("soldier_page1.png"),
			textures_path("Soldier_page2.png"),
			textures_path("doctor_page1.png"),
			textures_path("doctor_page2.png"),
			textures_path("hacker_page1.png"),
			textures_path("hacker_page2.png"),

			textures_path("confirm_button1.png"),
			textures_path("confirm_button3.png"),
			textures_path("confirm_button2.png"),
			textures_path("Tape_blue.png"),
			textures_path("Tape_green.png"),
			textures_path("Tape_light_blue.png"),
			textures_path("Tape_orange.png"),
			textures_path("Tape_rainbow.png"),

			textures_path("doc_1.png"),
			textures_path("doc_2.png"),
			textures_path("doc_3.png"),

			textures_path("hacker_1.png"),
			textures_path("hacker_2.png"),
			textures_path("hacker_3.png")
	};

	std::array<GLuint, effect_count> effects;
	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, effect_count> effect_paths = {
		shader_path("coloured"),
		shader_path("egg"),
		shader_path("meshed"),
		shader_path("textured"),
		shader_path("text"),
		shader_path("ring"),
		shader_path("rectangle"),
		shader_path("box"),
		shader_path("global"),
		//shader_path("textured")
		shader_path("map"),
		shader_path("fovShader"),
	};

	std::array<GLuint, geometry_count> vertex_buffers;
	std::array<GLuint, geometry_count> index_buffers;
	std::array<Mesh, geometry_count> meshes;

public:
	// Initialize the window
	bool init(GLFWwindow* window);

	template <class T>
	void bindVBOandIBO(GEOMETRY_BUFFER_ID gid, std::vector<T> vertices, std::vector<uint16_t> indices);

	void initializeGlTextures();

	void initializeGlEffects();

	void initializeGlMeshes();

	Mesh& getMesh(GEOMETRY_BUFFER_ID id) { return meshes[(int)id]; };

	void initializeGlGeometryBuffers();

	void initializeMap();
	// Initialize the screen texture used as intermediate render target
	// The draw loop first renders to this texture, then it is used for the wind
	// shader
	bool initScreenTexture();

	// Destroy resources associated to one or all entities created by the system
	~RenderSystem();

	// Draw all entities
	void draw();

	void initializeFullscreenQuad();

	mat3 createProjectionMatrix();

	void initializeFOVQuad();

	// Text renderer
	TextRenderer text_renderer;
	void setTextPosition(Entity textEntity, vec2 newPosition);

	CameraSystem& getCameraSystem();
	// Window handle
	GLFWwindow* window;
private:
	// Internal drawing functions for each entity type
	void drawTexturedMesh(Entity entity, const mat3& projection);
	void drawText(Entity entity, const mat3& projection);
	void drawMap(Entity entity, const mat3& projection);
	void drawToScreen();

	bool checkWallNearby(vec2 position, float check_radius);

	// Text renderer
	//TextRenderer text_renderer;

	// Screen texture handles
	GLuint frame_buffer;
	GLuint off_screen_render_buffer_color;
	GLuint off_screen_render_buffer_depth;

	// map buffer
	GLuint m_map_VAO;
	GLuint m_map_VBO;

	//FOV
	GLuint fov_vbo;
    GLuint fov_ibo;
    GLuint fov_vao;

	Entity screen_state_entity;
};

bool loadEffectFromFile(
	const std::string& vs_path, const std::string& fs_path, GLuint& out_program);
