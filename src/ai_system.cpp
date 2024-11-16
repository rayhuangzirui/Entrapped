// internal
#include "ai_system.hpp"
#include "state_manager.hpp"
#include <iostream>
#include <sstream>

struct PathNode {
	vec2 p = { 0, 0};
	float g = 0;
	float h = 0;
	float w = 0;
};

PathNode createPathNode(vec2 p) {
	return { p, 0, 0, 0};
}

bool isTouched(Motion motion, vec2 p) {
	vec2 half_extents = { abs(motion.scale.x) / 2.0f, abs(motion.scale.y) / 2.0f };

	return p.x > (motion.position.x - half_extents.x) && p.x < (motion.position.x + half_extents.x)
		&& p.y >(motion.position.y - half_extents.y) && p.y < (motion.position.y + half_extents.y);
}

std::vector<vec2> findNeighbours(vec2 v) {
	std::vector<vec2> neighbours;
	for (int i = v.y - 1; i <= v.y + 1; i++) {
		for (int j = v.x - 1; j <= v.x + 1; j++) {
			if (j >= 0 && j < state.map_width && i >= 0 && i < state.map_height && vec2(j, i) != v) {
				neighbours.push_back(vec2(j, i));
			}
		}
	}
	return neighbours;
}

int countAgainstWall(vec2 v) {
	int walls = 1;
	for (int i = v.y - 1; i <= v.y + 1; i++) {
		for (int j = v.x - 1; j <= v.x + 1; j++) {
			if (j >= 0 && j < state.map_width && i >= 0 && i < state.map_height && vec2(j, i) != v) {
				if (state.map[i][j] != 0) {
					walls++;
				}
			}
		}
	}
	return walls;
}

vec2 worldToMapCoords(vec2 v) {
	vec2 coords = { 0, 0 };
	coords.x = clamp_m(floor(round_to_digits(v.x / 48.f, 6)), 0, state.map_width - 1);
	coords.y = clamp_m(floor(round_to_digits(v.y / 48.f, 6)), 0, state.map_height - 1);
	return coords;
}

// Modified A* Path Finding
// On the basis of classic path finding, the entity will prefer open areas
std::vector<vec2> AISystem::findPath(vec2 start, vec2 end) {

	std::vector<vec2> open_set;
	std::vector<vec2> closed_set;
	std::vector<vec2> path;
	std::vector<std::vector<PathNode>>cell_info(state.map_height, std::vector<PathNode>(state.map_width, createPathNode({ 0, 0 })));
	open_set.push_back(start);

	while (open_set.size() > 0) {
		// TODO: fix dead loop
		int idx = 0;
		vec2 current_coords = open_set[idx];
		int current_idx = 0;
		if (cell_info[current_coords.y][current_coords.x].h <= 0) {
			cell_info[current_coords.y][current_coords.x].h = distance(end, current_coords);
		}
		for (vec2 v : open_set) {
			float f = cell_info[v.y][v.x].g + cell_info[v.y][v.x].h * cell_info[v.y][v.x].w;
			float cur_f = cell_info[current_coords.y][current_coords.x].g + cell_info[current_coords.y][current_coords.x].h * cell_info[current_coords.y][current_coords.x].w;
			if (f < cur_f) {
				current_coords = v;
				current_idx = idx;
			}
			idx++;
		}
		open_set.erase(open_set.begin() + current_idx);
		closed_set.push_back(current_coords);

		//std::cout << "is running pathfinding: " << current_coords.x << ": " << current_coords.y << std::endl;

		// found destination, return
		if (current_coords == end) {
			while (current_coords != start) {
				path.push_back(current_coords);
				current_coords = cell_info[current_coords.y][current_coords.x].p;
			}
			return path;
		}

		// generate neighbours
		std::vector<vec2> neighbours = findNeighbours(current_coords);
		for (vec2 neighbour : neighbours) {
			if (state.map[neighbour.y][neighbour.x] != 0 || std::find(closed_set.begin(), closed_set.end(), neighbour) != closed_set.end()) {
				continue;
			}

			// fill in info
			PathNode& neighbour_node = cell_info[neighbour.y][neighbour.x];
			PathNode& current_node = cell_info[current_coords.y][current_coords.x];
			// 
			neighbour_node.g = current_node.g + distance(neighbour, current_coords);
			neighbour_node.h = distance(end, neighbour);
			neighbour_node.p = current_coords;
			neighbour_node.w = countAgainstWall(neighbour);

			// skip if neighbour is more expensive
			if (std::find(open_set.begin(), open_set.end(), neighbour) != open_set.end()
				&& neighbour_node.g > current_node.g) {
				continue;
			}
			open_set.push_back(neighbour);
		}
	}
	return path;
}

void AISystem::step(float elapsed_ms_since_last_update)
{
	auto& enemyAI_container = registry.enemyAIs;
	auto& motion_container = registry.motions;
	auto& player_container = registry.players;
	if (player_container.size() <= 0) {
		return;
	}
	//std::vector<vec2> path = findPath({ 2, 2 }, { 4, 4 });
	//std::cout << path[1].x << ", " << path[1].y << std::endl;
	Entity player = player_container.entities[0];
	for (uint i = 0; i < enemyAI_container.components.size(); i++) {
		EnemyAI& enemyAI = enemyAI_container.components[i];
		Entity& entity = enemyAI_container.entities[i];
		Motion& motion = motion_container.get(entity);
		Motion& player_motion = motion_container.get(player);
		Health& health = registry.healths.get(entity);

		// states - root of the decision tree
		// wandering state: 0
		// chasing state: 1
		// fleeing state: 2
		if (enemyAI.state == 0) {
			// if the player is within a certain distance, switch to chasing
			if (distance(player_motion.position, motion.position) <= enemyAI.detection_radius) {
				vec2 start = worldToMapCoords(motion.position);
				vec2 end = worldToMapCoords(player_motion.position);
				enemyAI.path = findPath(start, end);

				enemyAI.state = 1;
				enemyAI.wander_timer = 0;
				enemyAI.last_player_position = player_motion.position;
				enemyAI.path_finding_timer = 0;
				continue;
			}
			// else, pick a random direction every 2 seconds
			else {
				enemyAI.wander_timer -= elapsed_ms_since_last_update;
				if (enemyAI.wander_timer <= 0) {
					// Randomize direction
					float random_x = (rand() % 200 - 100) / 100.f; // Random number between -1 and 1
					float random_y = (rand() % 200 - 100) / 100.f; // Random number between -1 and 1

					vec2 random_direction = normalize(vec2(random_x, random_y));
					motion.velocity = random_direction * 100.f;

					enemyAI.wander_timer = 2000.f;
				}
			}
		}
		else if (enemyAI.state == 1) {
			// if enemy's hp is lower than 30%, switch to fleeing
			if ((float)health.current_health / (float)health.max_health < 0.30) {
				enemyAI.state = 2;
				enemyAI.flee_timer = 3000.f;
				enemyAI.path.clear();
				continue;
			}

			// move to the last position where the player is seen.
			//vec2 direction = normalize(enemyAI.last_player_position - motion.position);
			//motion.velocity = direction * 100.f;
			
			// move to the last path node
			if (enemyAI.path.size() > 0) {
				vec2 last_path_position = ((enemyAI.path[enemyAI.path.size() - 1] + 0.5f)*48.f);
				vec2 direction = normalize(last_path_position - motion.position);
				motion.velocity = direction * 100.f;

				while (isTouched(motion, last_path_position)) {
					enemyAI.path.pop_back();
					if (enemyAI.path.size() <= 0) {
						break;
					}
					last_path_position = ((enemyAI.path[enemyAI.path.size() - 1] + 0.5f) * 48.f);
				}
			}
			else {
				motion.velocity = { 0, 0 };
			}

			// update path_finding_timer
			if (enemyAI.path_finding_timer > 0) {
				enemyAI.path_finding_timer -= elapsed_ms_since_last_update;
			}

			// find path to the last seen position
			if (enemyAI.path_finding_timer <= 0) {
				vec2 start = worldToMapCoords(motion.position);
				vec2 end = worldToMapCoords(enemyAI.last_player_position);
				enemyAI.path = findPath(start, end);
				enemyAI.path_finding_timer = 750.f;
			}

			// if the player is outside of the radius for 3 seconds
			// return to the wander state
			if (distance(player_motion.position, motion.position) >= enemyAI.detection_radius) {
				enemyAI.chase_timer -= elapsed_ms_since_last_update;
				if (enemyAI.chase_timer <= 0) {
					enemyAI.state = 0;
					enemyAI.chase_timer = 3000.f;
					enemyAI.path.clear();
				}
			}
			else { // else, update the position of the player for the AI
				enemyAI.last_player_position = player_motion.position;
				enemyAI.chase_timer = 3000.f;
			}

		}
		else if (enemyAI.state == 2) {
			// move away from the last position where the player is seen.
			vec2 direction = normalize(motion.position - enemyAI.last_player_position);
			motion.velocity = direction * 100.f;

			// if the player is outside of the radius for 3 seconds
			// return to the wander state
			if (distance(player_motion.position, motion.position) >= enemyAI.detection_radius) {
				enemyAI.flee_timer -= elapsed_ms_since_last_update;
				if (enemyAI.flee_timer <= 0) {
					enemyAI.state = 0;
					enemyAI.flee_timer = 3000.f;
				}
			}
			else { // else, update the position of the player for the AI
				enemyAI.last_player_position = player_motion.position;
				enemyAI.flee_timer = 3000.f;
			}
		}
	}
}