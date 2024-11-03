// camera_system.hpp
#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include <glm/glm.hpp>

class CameraSystem {
public:
    // Initializes the camera with default zoom and position
    CameraSystem();

    // Center the camera on the player and adjust based on zoom level
    void updateCamera(const vec2& player_position, int window_width, int window_height);

    // Adjust the zoom level
    void adjustZoom(float delta);

    // Get the current projection matrix for rendering
    mat3 getProjectionMatrix() const;

    mat3 getViewportMatrix(int window_width, int window_height) const;
    mat3 getDefaultMatrix() const;
private:
    vec2 position;
    float zoom_level;
    int window_width;
    int window_height;
};