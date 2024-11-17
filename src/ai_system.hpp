#pragma once

#include <vector>

#include "tiny_ecs_registry.hpp"
#include "common.hpp"

class AISystem
{
public:
	void step(float elapsed_ms);
private:
	std::vector<vec2> findPath(vec2 start, vec2 end);
};