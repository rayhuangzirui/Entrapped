#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"

class SceneSystem
{
public: 
	SceneSystem();
	Entity current_scene;
};

// the main menu
Entity createMainMenuScene();

// the game scene
Entity createGameScene();