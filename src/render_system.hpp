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
		// specify meshes of other assets here
	};

	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, texture_count> texture_paths = {
			textures_path("player_1_new.png"),
			textures_path("player_3_new.png"),
			textures_path("player_2_new.png"),

			/*textures_path("player_back_1.png"),
			textures_path("player_back_2.png"),
			textures_path("player_back_3.png"),

			textures_path("player_front_1.png"),
			textures_path("player_front_2.png"),
			textures_path("player_front_3.png"),*/

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

			textures_path("puffer_fish.png"),

			textures_path("pistol.png"),
			textures_path("SMG.png"),
			textures_path("rifle.png"),
	};

	std::array<GLuint, effect_count> effects;
	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, effect_count> effect_paths = {
		shader_path("coloured"),
		shader_path("textured"),
		shader_path("text"),
		shader_path("ring"),
		shader_path("box"),
		shader_path("global")
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
	// Initialize the screen texture used as intermediate render target
	// The draw loop first renders to this texture, then it is used for the wind
	// shader
	bool initScreenTexture();

	// Destroy resources associated to one or all entities created by the system
	~RenderSystem();

	// Draw all entities
	void draw();

	mat3 createProjectionMatrix();

	// Text renderer
	TextRenderer text_renderer;
	void setTextPosition(Entity textEntity, vec2 newPosition);
private:
	// Internal drawing functions for each entity type
	void drawTexturedMesh(Entity entity, const mat3& projection);
	void drawText(Entity entity, const mat3& projection);
	void drawToScreen();

	// Window handle
	GLFWwindow* window;

	// Text renderer
	//TextRenderer text_renderer;

	// Screen texture handles
	GLuint frame_buffer;
	GLuint off_screen_render_buffer_color;
	GLuint off_screen_render_buffer_depth;

	Entity screen_state_entity;
};

bool loadEffectFromFile(
	const std::string& vs_path, const std::string& fs_path, GLuint& out_program);
