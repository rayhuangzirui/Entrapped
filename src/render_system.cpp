// internal
#include "render_system.hpp"
#include <SDL.h>

#include "tiny_ecs_registry.hpp"


#include "camera_system.hpp"
#include "text_renderer.hpp"
#include <iostream>
#include <glm/ext/matrix_clip_space.hpp>

#include "state_manager.hpp"


void RenderSystem::drawTexturedMesh(Entity entity,
									const mat3 &projection)
{
	Motion &motion = registry.motions.get(entity);
	// Transformation code, see Rendering and Transformation in the template
	// specification for more info Incrementally updates transformation matrix,
	// thus ORDER IS IMPORTANT
	Transform transform;
	transform.translate(motion.position);
	transform.rotate(motion.angle);
	transform.scale(motion.scale);

	// !!! TODO A1: add rotation to the chain of transformations, mind the order
	// of transformations

	assert(registry.renderRequests.has(entity));
	const RenderRequest &render_request = registry.renderRequests.get(entity);

	const GLuint used_effect_enum = (GLuint)render_request.used_effect;
	assert(used_effect_enum != (GLuint)EFFECT_ASSET_ID::EFFECT_COUNT);
	const GLuint program = (GLuint)effects[used_effect_enum];

	// Setting shaders
	glUseProgram(program);
	gl_has_errors();

	assert(render_request.used_geometry != GEOMETRY_BUFFER_ID::GEOMETRY_COUNT);
	const GLuint vbo = vertex_buffers[(GLuint)render_request.used_geometry];
	const GLuint ibo = index_buffers[(GLuint)render_request.used_geometry];

	// Setting vertex and index buffers
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	gl_has_errors();

	// Input data location as in the vertex buffer
	if (render_request.used_effect == EFFECT_ASSET_ID::TEXTURED)
	{
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_texcoord_loc = glGetAttribLocation(program, "in_texcoord");
		gl_has_errors();
		assert(in_texcoord_loc >= 0);

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
							  sizeof(TexturedVertex), (void *)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_texcoord_loc);
		glVertexAttribPointer(
			in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex),
			(void *)sizeof(
				vec3)); // note the stride to skip the preceeding vertex position

		// Enabling and binding texture to slot 0
		glActiveTexture(GL_TEXTURE0);
		gl_has_errors();

		assert(registry.renderRequests.has(entity));
		GLuint texture_id =
			texture_gl_handles[(GLuint)registry.renderRequests.get(entity).used_texture];

		glBindTexture(GL_TEXTURE_2D, texture_id);
		gl_has_errors();

		// Handle light up effect for the player
		// Light up?
		GLint light_up_uloc = glGetUniformLocation(program, "light_up");
		assert(light_up_uloc >= 0);

		// !!! TODO A1: set the light_up shader variable using glUniform1i,
		// similar to the glUniform1f call below. The 1f or 1i specified the type, here a single int.
		/*if (registry.lightUps.has(entity)) {
			glUniform1i(light_up_uloc, 1);
		}
		else {
			glUniform1i(light_up_uloc, 0);
		}*/
		bool is_light_up = registry.lightups.has(entity);
		glUniform1i(light_up_uloc, is_light_up ? 1 : 0);
		gl_has_errors();


		if (registry.opacities.has(entity)) {
			// Set the opacity
			GLint opacity_uloc = glGetUniformLocation(program, "opacity");
			assert(opacity_uloc >= 0);
			glUniform1f(opacity_uloc, registry.opacities.get(entity).opacity);
		}
		else {
			// Set the opacity
			GLint opacity_uloc = glGetUniformLocation(program, "opacity");
			assert(opacity_uloc >= 0);
			glUniform1f(opacity_uloc, 1.0f);
		}
	}
	else if (render_request.used_effect == EFFECT_ASSET_ID::MESHED || render_request.used_effect == EFFECT_ASSET_ID::DEBUG_LINE || render_request.used_effect == EFFECT_ASSET_ID::RING || render_request.used_effect == EFFECT_ASSET_ID::BOX)
	{
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_color_loc = glGetAttribLocation(program, "in_color");
		gl_has_errors();

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
							  sizeof(ColoredVertex), (void *)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_color_loc);
		glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE,
							  sizeof(ColoredVertex), (void *)sizeof(vec3));
		gl_has_errors();
		if (render_request.used_effect == EFFECT_ASSET_ID::MESHED)
		{
			// Light up?
			GLint light_up_uloc = glGetUniformLocation(program, "light_up");
			assert(light_up_uloc >= 0);

			// !!! TODO A1: set the light_up shader variable using glUniform1i,
			// similar to the glUniform1f call below. The 1f or 1i specified the type, here a single int.
			gl_has_errors();
		}

		if (render_request.used_effect == EFFECT_ASSET_ID::BOX) {
			if (registry.opacities.has(entity)) {
				// Set the opacity
				GLint opacity_uloc = glGetUniformLocation(program, "opacity");
				assert(opacity_uloc >= 0);
				glUniform1f(opacity_uloc, registry.opacities.get(entity).opacity);
			}
			else {
				// Set the opacity
				GLint opacity_uloc = glGetUniformLocation(program, "opacity");
				assert(opacity_uloc >= 0);
				glUniform1f(opacity_uloc, 1.0f);
			}
		}
	}
	else if (render_request.used_effect == EFFECT_ASSET_ID::FOV_NEW) {
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_color_loc = glGetAttribLocation(program, "in_color");
		gl_has_errors();

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
			sizeof(ColoredVertex), (void*)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_color_loc);
		glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE,
			sizeof(ColoredVertex), (void*)sizeof(vec3));
		gl_has_errors();

		int map_len = 30;
		int map[30 * 30];
		int start_x = floor((motion.position.x - motion.scale.x/2.f)/48.f);
		int start_y = floor((motion.position.y - motion.scale.y / 2.f) / 48.f);
		for (int i = 0; i < map_len; i++) {
			for (int j = 0; j < map_len; j++) {
				float ci = start_y + i;
				float cj = start_x + j;
				if (ci < 0 || ci >= state.map_height || cj < 0 || cj >= state.map_width) {
					map[j + map_len * i] = 1;
				}
				else {
					map[j + map_len * i] = state.map.collision_layer[ci][cj] == 0 ? 0 : 1;
				}
			}
		}
		//std::cout << "fov position x: " << motion.position.x << std::endl;
		//std::cout << "fov position y: " << motion.position.y << std::endl;
		GLint mapLoc = glGetUniformLocation(program, "map");
		if (mapLoc >= 0) {
			glUniform1iv(mapLoc, map_len * map_len, map);
		}
		gl_has_errors();

		// pass in world offset
		vec2 world_offset = { start_x *48.f, start_y*48.f};
		glUniform2fv(glGetUniformLocation(program, "world_offset"), 1, (float*)&world_offset);
		gl_has_errors();

		// pass in the center position
		glUniform2fv(glGetUniformLocation(program, "world_position"), 1, (float*)&motion.position);
		gl_has_errors();

	}
	else if (render_request.used_effect == EFFECT_ASSET_ID::COLOURED)
{
    GLint in_position_loc = glGetAttribLocation(program, "in_position");
    gl_has_errors();

    
    glEnableVertexAttribArray(in_position_loc);
    glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
                          sizeof(ColoredVertex), (void *)0);
    gl_has_errors();

    // Setting the color using a uniform
    if (registry.colors.has(entity)) {
        const vec3 color = registry.colors.get(entity);
        glUniform3fv(glGetUniformLocation(program, "color"), 1, (float *)&color);
    } else {
        vec3 default_color = vec3(1.f, 1.f, 1.f);  // Default to white
        glUniform3fv(glGetUniformLocation(program, "color"), 1, (float *)&default_color);
    }
}


//2

else if (render_request.used_effect == EFFECT_ASSET_ID::FOV2) 
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(program);

    // Use FOV quad geometry
    const GLuint vbo = vertex_buffers[(GLuint)GEOMETRY_BUFFER_ID::FOV_QUAD];
    const GLuint ibo = index_buffers[(GLuint)GEOMETRY_BUFFER_ID::FOV_QUAD];

    // Bind buffers
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    // Set up vertex attribute
    GLint in_position_loc = glGetAttribLocation(program, "in_position");
    if (in_position_loc >= 0) {
        glEnableVertexAttribArray(in_position_loc);
        glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
                             sizeof(ColoredVertex), (void*)0);
    }

    // Check for text first, independent of player
    GLint isTextLoc = glGetUniformLocation(program, "is_text");
    if (isTextLoc >= 0) {
        // Check if this specific entity is text
        bool isText = registry.texts.has(entity);
        glUniform1i(isTextLoc, isText);
    }

    // Set player-related uniforms
    if (!registry.players.entities.empty()) {
        Entity player_entity = registry.players.entities[0];
        if (registry.motions.has(player_entity)) {
            Motion& player_motion = registry.motions.get(player_entity);
            
            // Get window size
            int window_width, window_height;
            glfwGetFramebufferSize(window, &window_width, &window_height);
            
            // Calculate player's screen position
            vec2 player_screen_pos = vec2(
                window_width/2,  // Center of screen X
                window_height/2  // Center of screen Y
            );
            
            GLint playerPosLoc = glGetUniformLocation(program, "playerPosition");
            if (playerPosLoc >= 0) {
                glUniform2f(playerPosLoc, player_screen_pos.x, player_screen_pos.y);
            }

            GLint radiusLoc = glGetUniformLocation(program, "circleRadius");
            if (radiusLoc >= 0) {
                float circle_radius = 500.0f;
                glUniform1f(radiusLoc, circle_radius);
            }

			GLuint time_uloc = glGetUniformLocation(program, "time");
			if (radiusLoc >= 0) {
				glUniform1f(time_uloc, (float)(glfwGetTime() * 10.0f));
			}

            GLint windowSizeLoc = glGetUniformLocation(program, "windowSize");
            if (windowSizeLoc >= 0) {
                glUniform2f(windowSizeLoc, (float)window_width, (float)window_height);
            }
        }
    }

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}


  
	
	else
	{
	std::cerr << "Unsupported RenderRequest encountered!" << std::endl;
    std::cerr << "RenderRequest used_effect: " << (GLuint)render_request.used_effect << std::endl;
    std::cerr << "RenderRequest used_texture: " << (GLuint)render_request.used_texture << std::endl;
    std::cerr << "RenderRequest used_geometry: " << (GLuint)render_request.used_geometry << std::endl;

		assert(false && "Type of render request not supported");
	}

	// Getting uniform locations for glUniform* calls
	GLint color_uloc = glGetUniformLocation(program, "fcolor");
	const vec3 color = registry.colors.has(entity) ? registry.colors.get(entity) : vec3(1);
	glUniform3fv(color_uloc, 1, (float *)&color);
	gl_has_errors();

	// Get number of indices from index buffer, which has elements uint16_t
	GLint size = 0;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	gl_has_errors();

	GLsizei num_indices = size / sizeof(uint16_t);
	// GLsizei num_triangles = num_indices / 3;

	GLint currProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);
	// Setting uniform values to the currently bound program
	GLuint transform_loc = glGetUniformLocation(currProgram, "transform");
	glUniformMatrix3fv(transform_loc, 1, GL_FALSE, (float *)&transform.mat);
	GLuint projection_loc = glGetUniformLocation(currProgram, "projection");
	glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float *)&projection);
	gl_has_errors();
	// Drawing of num_indices/3 triangles specified in the index buffer
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
	gl_has_errors();
}

void RenderSystem::drawText(Entity entity, const mat3& projection) {
	Motion& motion = registry.motions.get(entity);
	// Transformation code, see Rendering and Transformation in the template
	// specification for more info Incrementally updates transformation matrix,
	// thus ORDER IS IMPORTANT
	Transform transform;
	transform.translate(motion.position);
	transform.scale(motion.scale);
	// !!! TODO A1: add rotation to the chain of transformations, mind the order
	// of transformations

	// Entity text_fov_entity = Entity();
    // Motion& text_fov_motion = registry.motions.emplace(text_fov_entity);
    // registry.fovs.emplace(text_fov_entity);
    // text_fov_motion = registry.motions.get(text_fov_entity);

	assert(registry.renderRequests.has(entity));
	const RenderRequest& render_request = registry.renderRequests.get(entity);

	const GLuint used_effect_enum = (GLuint)render_request.used_effect;
	assert(used_effect_enum != (GLuint)EFFECT_ASSET_ID::EFFECT_COUNT);
	const GLuint program = (GLuint)effects[used_effect_enum];

	// Setting shaders
	glUseProgram(program);
	gl_has_errors();

	assert(render_request.used_geometry != GEOMETRY_BUFFER_ID::GEOMETRY_COUNT);
	const GLuint vbo = vertex_buffers[(GLuint)render_request.used_geometry];
	const GLuint ibo = index_buffers[(GLuint)render_request.used_geometry];

	// Setting vertex and index buffers
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	gl_has_errors();

	// Input data location as in the vertex buffer
	if (render_request.used_effect == EFFECT_ASSET_ID::TEXT)
	{
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_texcoord_loc = glGetAttribLocation(program, "in_texcoord");
		gl_has_errors();
		assert(in_texcoord_loc >= 0);

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
			sizeof(TexturedVertex), (void*)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_texcoord_loc);
		glVertexAttribPointer(
			in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex),
			(void*)sizeof(
				vec3)); // note the stride to skip the preceeding vertex position

		// Enabling and binding texture to slot 0
		glActiveTexture(GL_TEXTURE0);
		gl_has_errors();

		if (registry.opacities.has(entity)) {
			// Set the opacity
			GLint opacity_uloc = glGetUniformLocation(program, "opacity");
			assert(opacity_uloc >= 0);
			glUniform1f(opacity_uloc, registry.opacities.get(entity).opacity);
		}
		else {
			// Set the opacity
			GLint opacity_uloc = glGetUniformLocation(program, "opacity");
			assert(opacity_uloc >= 0);
			glUniform1f(opacity_uloc, 1.0f);
		}
		gl_has_errors();

	}
	else
	{
		assert(false && "Type of render request not supported");
	}

	// Getting uniform locations for glUniform* calls
	GLint color_uloc = glGetUniformLocation(program, "fcolor");
	const vec3 color = registry.colors.has(entity) ? registry.colors.get(entity) : vec3(1);
	glUniform3fv(color_uloc, 1, (float*)&color);
	gl_has_errors();

	// Get number of indices from index buffer, which has elements uint16_t
	GLint size = 0;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	gl_has_errors();

	GLsizei num_indices = size / sizeof(uint16_t);
	// GLsizei num_triangles = num_indices / 3;

	GLint currProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);
	// Setting uniform values to the currently bound program
	GLuint transform_loc = glGetUniformLocation(currProgram, "transform");
	glUniformMatrix3fv(transform_loc, 1, GL_FALSE, (float*)&transform.mat);
	GLuint projection_loc = glGetUniformLocation(currProgram, "projection");
	glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float*)&projection);
	gl_has_errors();

	assert(registry.texts.has(entity));
	std::string text = registry.texts.get(entity).content;
	std::string::const_iterator c;
	float x = 0.0;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = this->text_renderer.Characters[*c];
		GLfloat xpos = x + ch.Bearing.x * motion.scale.x/40.f;
		GLfloat ypos = (ch.Size.y - ch.Bearing.y) * motion.scale.y / 40.f;
		float w = ch.Size.x * 1.0f;
		float h = ch.Size.y * 1.0f;
		GLuint xpos_loc = glGetUniformLocation(currProgram, "xpos");
		glUniform1f(xpos_loc, (float)xpos);
		GLuint ypos_loc = glGetUniformLocation(currProgram, "ypos");
		glUniform1f(ypos_loc, (float)ypos);
		GLuint w_ratio_loc = glGetUniformLocation(currProgram, "w_ratio");
		glUniform1f(w_ratio_loc, (float)w/40.f);
		GLuint h_ratio_loc = glGetUniformLocation(currProgram, "h_ratio");
		glUniform1f(h_ratio_loc, (float)h/40.f);
		gl_has_errors();
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		gl_has_errors();
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
		x += (ch.Advance >> 6) * motion.scale.x/40.0f; //(2^6 = 64)
	}

	gl_has_errors();
}


//bool RenderSystem::checkWallNearby(vec2 position, float check_radius) {
//    
//    int col = static_cast<int>(position.x / 48.f);
//    int row = static_cast<int>(position.y / 48.f); 
//    int radius_tiles = static_cast<int>(check_radius / 48.f);
//    for (int r = row - radius_tiles; r <= row + radius_tiles; r++) {
//        for (int c = col - radius_tiles; c <= col + radius_tiles; c++) { 
//            if (r >= 0 && r < state.map_height && c >= 0 && c < state.map_width) { 
//                if (state.map[r][c] == 1) {
//                    vec2 wall_pos = vec2(c * 48.f + 24.f, r * 48.f + 24.f);
//                    float dist = length(position - wall_pos);
//                    if (dist < check_radius) {
//                        return true;
//                    }
//                }
//            }
//        }
//    }
//    return false;
//}

void RenderSystem::drawMap(Entity entity, const mat3& projection) {
	Motion& motion = registry.motions.get(entity);
	// Transformation code, see Rendering and Transformation in the template
	// specification for more info Incrementally updates transformation matrix,
	// thus ORDER IS IMPORTANT
	Transform transform;
	transform.translate(motion.position);
	transform.scale(motion.scale);

	assert(registry.renderRequests.has(entity));
	const RenderRequest& render_request = registry.renderRequests.get(entity);

	const GLuint used_effect_enum = (GLuint)render_request.used_effect;
	assert(used_effect_enum != (GLuint)EFFECT_ASSET_ID::EFFECT_COUNT);
	const GLuint program = (GLuint)effects[used_effect_enum];

	// Setting shaders
	glUseProgram(program);
	gl_has_errors();

	assert(render_request.used_geometry != GEOMETRY_BUFFER_ID::GEOMETRY_COUNT);
	const GLuint vbo = vertex_buffers[(GLuint)render_request.used_geometry];
	const GLuint ibo = index_buffers[(GLuint)render_request.used_geometry];

	// Setting vertex and index buffers
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	gl_has_errors();

	// Input data location as in the vertex buffer
	if (render_request.used_effect == EFFECT_ASSET_ID::MAP)
	{
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_texcoord_loc = glGetAttribLocation(program, "in_texcoord");
		gl_has_errors();
		assert(in_texcoord_loc >= 0);

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
			sizeof(TexturedVertex), (void*)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_texcoord_loc);
		glVertexAttribPointer(
			in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex),
			(void*)sizeof(
				vec3)); // note the stride to skip the preceeding vertex position

		// Enabling and binding texture to slot 0
		glActiveTexture(GL_TEXTURE0);
		gl_has_errors();

		gl_has_errors();

	}
	else
	{
		assert(false && "Type of render request not supported");
	}

	// Getting uniform locations for glUniform* calls
	GLint color_uloc = glGetUniformLocation(program, "fcolor");
	const vec3 color = registry.colors.has(entity) ? registry.colors.get(entity) : vec3(1);
	glUniform3fv(color_uloc, 1, (float*)&color);
	gl_has_errors();

	// Get number of indices from index buffer, which has elements uint16_t
	GLint size = 0;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	gl_has_errors();

	GLsizei num_indices = size / sizeof(uint16_t);
	// GLsizei num_triangles = num_indices / 3;

	GLint currProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);
	// Setting uniform values to the currently bound program
	GLuint transform_loc = glGetUniformLocation(currProgram, "transform");
	glUniformMatrix3fv(transform_loc, 1, GL_FALSE, (float*)&transform.mat);
	GLuint projection_loc = glGetUniformLocation(currProgram, "projection");
	glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float*)&projection);
	gl_has_errors();

	float x = 0.0;
	float y = 0.0;
	for (int row = 0; row < state.map_height; ++row) {
		for (int col = 0; col < state.map_width; ++col) {
			GLfloat xpos = x;
			GLfloat ypos = y;
			GLuint texture_id = texture_gl_handles[(GLuint)TEXTURE_ASSET_ID::WALL_6];
			//if (state.is_wall(state.map[row][col])) {
			//	//texture_id =
			//	//	texture_gl_handles[(GLuint)TEXTURE_ASSET_ID::WALL_6];
			//	texture_id = state.map_tile_textures[0];
			//}
			//else if (state.is_floor(state.map[row][col])) {
			//	texture_id =
			//		texture_gl_handles[(GLuint)TEXTURE_ASSET_ID::FLOOR_5];
			//}
			//else {
			//	x += 48.f;
			//	continue;
			//}
			if (state.map.decoration_layer[row][col] > -1) {
				texture_id = state.map_tile_textures[state.map.decoration_layer[row][col]];
			}
			else {
				x += 48.f;
				continue;
			}
			GLuint xpos_loc = glGetUniformLocation(currProgram, "xpos");
			glUniform1f(xpos_loc, (float)xpos);
			GLuint ypos_loc = glGetUniformLocation(currProgram, "ypos");
			glUniform1f(ypos_loc, (float)ypos);
			gl_has_errors();
			glBindTexture(GL_TEXTURE_2D, texture_id);
			gl_has_errors();
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
			x += 48.f;
		}
		y += 48.f;
		x = 0.0f;
	}

	gl_has_errors();
}

// draw the intermediate texture to the screen, with some distortion to simulate
// water
void RenderSystem::drawToScreen()
{
	// Setting shaders
	// get the water texture, sprite mesh, and program
	glUseProgram(effects[(GLuint)EFFECT_ASSET_ID::GLOBAL ]);
	gl_has_errors();
	// Clearing backbuffer
	int w, h;
	glfwGetFramebufferSize(window, &w, &h); // Note, this will be 2x the resolution given to glfwCreateWindow on retina displays
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, w, h);
	glDepthRange(0, 10);
	glClearColor(1.f, 0, 0, 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gl_has_errors();
	// Enabling alpha channel for textures
	glDisable(GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	// Draw the screen texture on the quad geometry
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]);
	glBindBuffer(
		GL_ELEMENT_ARRAY_BUFFER,
		index_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]); // Note, GL_ELEMENT_ARRAY_BUFFER associates
																	 // indices to the bound GL_ARRAY_BUFFER
	gl_has_errors();
	const GLuint water_program = effects[(GLuint)EFFECT_ASSET_ID::GLOBAL];
	// Set clock
	GLuint time_uloc = glGetUniformLocation(water_program, "time");
	GLuint dead_timer_uloc = glGetUniformLocation(water_program, "darken_screen_factor");
	glUniform1f(time_uloc, (float)(glfwGetTime() * 10.0f));
	ScreenState &screen = registry.screenStates.get(screen_state_entity);
	glUniform1f(dead_timer_uloc, screen.darken_screen_factor);
	gl_has_errors();
	// Set the vertex position and vertex texture coordinates (both stored in the
	// same VBO)
	GLint in_position_loc = glGetAttribLocation(water_program, "in_position");
	glEnableVertexAttribArray(in_position_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void *)0);
	gl_has_errors();

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, off_screen_render_buffer_color);
	gl_has_errors();
	// Draw
	glDrawElements(
		GL_TRIANGLES, 3, GL_UNSIGNED_SHORT,
		nullptr); // one triangle = 3 vertices; nullptr indicates that there is
				  // no offset from the bound index buffer
	gl_has_errors();
}

// Render our game world
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
void RenderSystem::draw()
{
	// Getting size of window
	int w, h;
	glfwGetFramebufferSize(window, &w, &h); // Note, this will be 2x the resolution given to glfwCreateWindow on retina displays


	//--------------------------Camera--------------------------//
	// Ensure player entity is valid and retrieve its position
	if (registry.players.entities.size() > 0) {
		Entity player = registry.players.entities[0];
		if (registry.motions.has(player)) {
			vec2 player_position = registry.motions.get(player).position;

			// Update the camera to center on the player¡¯s position
			camera_system.updateCamera(player_position, w, h);
		}
	}

	// Get the camera matrix after update
	mat3 camera_matrix = camera_system.getViewportMatrix(w, h);
	//--------------------------Camera--------------------------//

	// First render to the custom framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	gl_has_errors();
	// Clearing backbuffer
	glViewport(0, 0, w, h);
	glDepthRange(0.00001, 10);
	glClearColor(GLfloat(0 / 255), GLfloat(0 / 255), GLfloat(0 / 255), 1.0);
	glClearDepth(10.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST); // native OpenGL does not work with a depth buffer
							  // and alpha blending, one would have to sort
							  // sprites back to front
	gl_has_errors();
	mat3 projection_2D = createProjectionMatrix();
	// Draw all textured meshes that have a position and size component
	//for (Entity entity : registry.renderRequests.entities)
	//{
	//	if (!registry.motions.has(entity))
	//		continue;
	//	// Note, its not very efficient to access elements indirectly via the entity
	//	// albeit iterating through all Sprites in sequence. A good point to optimize
	//	if (!registry.texts.has(entity)) {
	//		drawTexturedMesh(entity, projection_2D);
	//	}
	//	else {
	//		drawText(entity, projection_2D);
	//	}
	//}

	// Use the camera matrix for all entities
	// for (Entity entity : registry.renderRequests.entities)
	// {
	// 	//if (!registry.motions.has(entity))
	// 	//	continue;
	// 	if (registry.cameraTexts.has(entity)) {
	// 		drawText(entity, camera_matrix);
	// 	}
	// 	else if (registry.texts.has(entity)) {
	// 		drawText(entity, projection_2D);
	// 	}
	// 	else if (registry.maps.has(entity)) {
	// 		drawMap(entity, camera_matrix);
	// 	}
	// 	else if (registry.UIs.has(entity) || registry.backgrounds.has(entity)) {
	// 		drawTexturedMesh(entity, projection_2D);
	// 	}
	// 	else {
	// 		drawTexturedMesh(entity, camera_matrix);
	// 	}
	// }



for (Entity entity : registry.renderRequests.entities) {
    if (!registry.fovs.has(entity) && !registry.texts.has(entity) && !registry.UIs.has(entity)) {
        if (registry.maps.has(entity)) {
            drawMap(entity, camera_matrix);
        } else if (registry.backgrounds.has(entity)) {
            drawTexturedMesh(entity, projection_2D);
        } else {
            drawTexturedMesh(entity, camera_matrix);
        }
    }
}


for (Entity entity : registry.renderRequests.entities) {
    if (registry.fovs.has(entity)) {
        drawTexturedMesh(entity, camera_matrix);
		//drawTexturedMesh(entity, projection_2D);
    }
}


for (Entity entity : registry.renderRequests.entities) {
    if (registry.texts.has(entity)) {
        if (registry.cameraTexts.has(entity)) {
            drawText(entity, camera_matrix);
        } else {
            drawText(entity, projection_2D);
        }
	}
	else if (registry.UIs.has(entity)) {
		drawTexturedMesh(entity, projection_2D);
	}
}

	// Truely render to the screen
	drawToScreen();

	// flicker-free display with a double buffer
	glfwSwapBuffers(window);
	gl_has_errors();
}

void RenderSystem::initializeFullscreenQuad() {
    std::vector<ColoredVertex> vertices = {
        {{-1.0f, -1.0f, 0.0f}},  // Bottom left
        {{1.0f, -1.0f, 0.0f}},   // Bottom right
        {{1.0f, 1.0f, 0.0f}},    // Top right
        {{-1.0f, 1.0f, 0.0f}}    // Top left
    };
    
    std::vector<uint16_t> indices = {
        0, 1, 2,    // First triangle
        0, 2, 3     // Second triangle
    };
    
    bindVBOandIBO(GEOMETRY_BUFFER_ID::SPRITE, vertices, indices);
}

mat3 RenderSystem::createProjectionMatrix()
{
	// Fake projection matrix, scales with respect to window coordinates
	float left = 0.f;
	float top = 0.f;

	gl_has_errors();
	float right = (float) window_width_px;
	float bottom = (float) window_height_px;

	float sx = 2.f / (right - left);
	float sy = 2.f / (top - bottom);
	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);
	return {{sx, 0.f, 0.f}, {0.f, sy, 0.f}, {tx, ty, 1.f}};
}


// Add this function to initialize the fullscreen quad
// void RenderSystem::initializeFOVQuad() {
//     // Create and bind VAO
//     glGenVertexArrays(1, &fov_vao);
//     glBindVertexArray(fov_vao);

//     // Vertices for a fullscreen quad
//     float quad_vertices[] = {
//         -1.0f, -1.0f, 0.0f,
//          1.0f, -1.0f, 0.0f,
//          1.0f,  1.0f, 0.0f,
//         -1.0f,  1.0f, 0.0f
//     };

//     // Indices
//     unsigned int quad_indices[] = {
//         0, 1, 2,
//         0, 2, 3
//     };

//     // Create and bind buffers
//     glGenBuffers(1, &fov_vbo);
//     glGenBuffers(1, &fov_ibo);

//     // Set up vertex buffer
//     glBindBuffer(GL_ARRAY_BUFFER, fov_vbo);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);

//     // Set up index buffer
//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fov_ibo);
//     glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_indices), quad_indices, GL_STATIC_DRAW);
// }

void RenderSystem::initializeFOVQuad() {
    // Define the quad vertices for a fullscreen quad
    std::vector<ColoredVertex> vertices = {
        {{-1.0f, -1.0f, 0.0f}}, // Bottom left
        {{ 1.0f, -1.0f, 0.0f}}, // Bottom right
        {{ 1.0f,  1.0f, 0.0f}}, // Top right
        {{-1.0f,  1.0f, 0.0f}}  // Top left
    };

    std::vector<uint16_t> indices = {
        0, 1, 2,  // First triangle
        0, 2, 3   // Second triangle
    };

    // Add new geometry buffer ID for FOV quad
    bindVBOandIBO(GEOMETRY_BUFFER_ID::FOV_QUAD, vertices, indices);
}

void RenderSystem::setTextPosition(Entity textEntity, vec2 newPosition) {
	if (registry.motions.has(textEntity)) {
		Motion& motion = registry.motions.get(textEntity);
		motion.position = newPosition;
	}
}

CameraSystem& RenderSystem::getCameraSystem() {
	return camera_system;
}