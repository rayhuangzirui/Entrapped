// camera_system.cpp
#include "camera_system.hpp"
#include "scenes/main_menu.hpp"
#include <algorithm>
#include <iostream>

// Constructor with default values
CameraSystem::CameraSystem() : position(vec2(0.f, 0.f)), zoom_level(1.0f), window_width(800), window_height(600) {}

void CameraSystem::updateCamera(const vec2& player_position, int window_width, int window_height) {
    // Center the camera on the player's position
    if (!MainMenu::in_main_menu) {
        position = player_position;
    }
}


// tabbed out since clamp doesnt work when theres no c++17
//void CameraSystem::adjustZoom(float delta) {
//    zoom_level += delta;
//    zoom_level = std::clamp(zoom_level, 0.5f, 2.0f); // Limits zoom between 0.5x and 2x
//}

void CameraSystem::adjustZoom(float delta) {
    float previous_zoom = zoom_level; // Store the previous zoom level
    zoom_level += delta;
    if (zoom_level < 0.5f) {
        zoom_level = 0.5f;
    }
    else if (zoom_level > 2.0f) {
        zoom_level = 2.0f;
    }

    // Print to console only if the zoom level changed
    if (zoom_level != previous_zoom) {
        if (delta > 0) {
            std::cout << "Zooming in. Current zoom level: " << zoom_level << "x" << std::endl;
        }
        else {
            std::cout << "Zooming out. Current zoom level: " << zoom_level << "x" << std::endl;
        }
    }
}

mat3 CameraSystem::getProjectionMatrix() const {
    float left = 0.f;
    float top = 0.f;
    float right = (float)window_width / zoom_level;
    float bottom = (float)window_height / zoom_level;

    float sx = 2.f / (right - left);
    float sy = 2.f / (top - bottom);
    float tx = -(right + left) / (right - left);
    float ty = -(top + bottom) / (top - bottom);

    return { {sx, 0.f, 0.f}, {0.f, sy, 0.f}, {tx, ty, 1.f} };
}

mat3 CameraSystem::getViewportMatrix(int window_width, int window_height) const {
    float half_width = (window_width / zoom_level) / 2.0f;
    float half_height = (window_height / zoom_level) / 2.0f;

    // Set up the translation to center on the player (camera position)
    float left = position.x - half_width;
    float right = position.x + half_width;
    float bottom = position.y + half_height;
    float top = position.y - half_height;

    // Create transformation matrix
    float sx = 2.f / (right - left);
    float sy = 2.f / (top - bottom);
    float tx = -(right + left) / (right - left);
    float ty = -(top + bottom) / (top - bottom);

    return { {sx, 0.f, 0.f}, {0.f, sy, 0.f}, {tx, ty, 1.f} };
}

mat3 CameraSystem::getDefaultMatrix() const {
    float left = 0.f;
    float top = 0.f;
    float right = 1280.f;
    float bottom = 720.f;

    float sx = 2.f / (right - left);
    float sy = 2.f / (top - bottom);
    float tx = -(right + left) / (right - left);
    float ty = -(top + bottom) / (top - bottom);

    return { {sx, 0.f, 0.f}, {0.f, sy, 0.f}, {tx, ty, 1.f} };
}