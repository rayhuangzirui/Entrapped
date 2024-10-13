#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "components.hpp"
#include "tiny_ecs_registry.hpp"

// A simple physics system that moves rigid bodies and checks for collision
class PhysicsSystem
{
public:
	//bool check_player_wall_collision(const Motion& player_motion);
	void step(float elapsed_ms);

	PhysicsSystem()
	{
	}
private:
	//const int TILE_SIZE = 48;
};