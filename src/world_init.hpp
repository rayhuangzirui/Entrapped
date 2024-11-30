#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"

// These are hardcoded to the dimensions of the entity texture
// BB = bounding box
const float PLAYER_BB_WIDTH = 0.6f * 165.f;
const float PLAYER_BB_HEIGHT = 0.6f * 165.f;
const float ENEMY_BB_WIDTH = 0.6f * 165.f;
const float ENEMY_BB_HEIGHT = 0.6f * 165.f;
const float BULLET_BB_WIDTH = 10.f;
const float BULLET_BB_HEIGHT = 10.f;

// a red line for debugging purposes
Entity createLine(vec2 position, vec2 size);

//a wall
Entity createWall(RenderSystem* renderer, vec2 position, vec2 size);

// background
Entity createBackground(RenderSystem* renderer);

Entity createChooseProfessionTitle(RenderSystem* renderer);

Entity createProfessionLayer(RenderSystem* renderer, vec2 pos, std::string profession, bool is_clicked);

Entity createConfirmButton(RenderSystem* renderer, vec2 pos, bool is_enabled, bool is_clicked);

Entity createButton(RenderSystem* renderer, vec2 pos, vec2 scale, std::string text);

Entity createMessage(RenderSystem* renderer, vec2 pos, std::string text);

// create a grid box of the screen
Entity createTransitionMask(RenderSystem* renderer, float progress);