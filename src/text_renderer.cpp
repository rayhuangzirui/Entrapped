/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "text_renderer.hpp"
#include "render_system.hpp"
#include "tiny_ecs_registry.hpp"
#include "tiny_ecs.hpp"
#include "common.hpp"
#include "camera_system.hpp"

TextRenderer::TextRenderer() {
    std::cout << ">>> Text Renderer Initialized" << std::endl;
}

int TextRenderer::loadFont() {
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return -1;
    }

    FT_Face face;
    if (FT_New_Face(ft, font_path("arial.ttf").c_str(), 0, &face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return -1;
    }

    FT_Set_Pixel_Sizes(face, 0, 48);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            //face->glyph->advance.x
            static_cast<unsigned int>(face->glyph->advance.x)
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    return 1;
}

Entity TextRenderer::createText(std::string txt, vec2 position, float scale, vec3 color) {
    auto entity = Entity();

    // Setting initial motion values
    Motion& motion = registry.motions.emplace(entity);
    motion.position = position;
    motion.angle = 0.f;
    motion.velocity = { 0.f, 0.f };
    motion.scale = {scale, scale};
    //motion.scale.y *= -1; // point front to the right

    // create an empty Salmon component for our character
    Text& text = registry.texts.emplace(entity);
    text.content = txt;
    registry.colors.insert(entity, color);
    registry.renderRequests.insert(
        entity,
        { TEXTURE_ASSET_ID::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no texture is needed
            EFFECT_ASSET_ID::TEXT,
            GEOMETRY_BUFFER_ID::SPRITE });
    //std::cout << ">>> Text created: " << txt << std::endl;
    return entity;
}

Entity TextRenderer::createTextInView(const std::string& txt, vec2 world_position, float scale, vec3 color, const CameraSystem& camera_system) {
    // Convert world position to screen position using the camera system's viewport matrix
    mat3 viewport_matrix = camera_system.getViewportMatrix(camera_system.window_width, camera_system.window_height);

    // Transform the world position to screen coordinates
    vec3 position_in_world_space = vec3(world_position.x, world_position.y, 1.0f);
    vec3 position_on_screen = viewport_matrix * position_in_world_space;

    // Create the text entity at the transformed screen position
    auto entity = Entity();
    Motion& motion = registry.motions.emplace(entity);
    motion.position = { position_on_screen.x, position_on_screen.y };
    motion.angle = 0.f;
    motion.velocity = { 0.f, 0.f };
    motion.scale = { scale, scale };

    // Initialize and configure the text component
    Text& text = registry.texts.emplace(entity);
    text.content = txt;
    registry.colors.insert(entity, color);
    registry.renderRequests.insert(
        entity,
        { TEXTURE_ASSET_ID::TEXTURE_COUNT, EFFECT_ASSET_ID::TEXT, GEOMETRY_BUFFER_ID::SPRITE }
    );

    return entity;
}

void TextRenderer::removeText(Entity text_entity) {
    if (registry.texts.has(text_entity)) {
        // Remove all components associated with the text entity
        registry.remove_all_components_of(text_entity);
        //std::cout << ">>> Text removed" << std::endl;
    }
    else {
        std::cout << ">>> Warning: Attempted to remove non-existent text entity" << std::endl;
    }
}

void TextRenderer::updateTextPosition(Entity text_entity, glm::vec2 new_position) {
    if (registry.motions.has(text_entity)) {
        // Update the position of the Motion component
        registry.motions.get(text_entity).position = new_position;
        //std::cout << ">>> Text position updated to: (" << new_position.x << ", " << new_position.y << ")" << std::endl;
    }
    else {
        std::cout << ">>> Warning: Attempted to update position of non-existent text entity" << std::endl;
    }
}