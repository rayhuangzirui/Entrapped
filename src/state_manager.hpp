#pragma once
#include <vector>
#include <array>
#include "common.hpp"
#include "map_system.hpp"

struct UpgradeState {
	int curVal = 0;
	int upgrade_amount = 0;
	int upgrade_cost = 0;
};

const int TILE_NUMBER = 26;

class StateManager
{
public:
	// map
	const int TILE_SIZE = 48;
	int map_height;
	int map_width;
	MapState map;
	uint map_index;
	std::vector<std::string> map_lists;
	std::array<GLuint, TILE_NUMBER> map_tile_textures;
	
	// save
	uint saved_map_index;
	int saved_max_health;
	int saved_health;
	int saved_ammo;

	int saved_ammo_pack;
	int saved_health_potion;

	// powerups
	int saved_speed_boost;
	int saved_shield;
	int saved_life_steal;
	int saved_ricochet;
	int saved_attack_speed;

	std::string saved_profession;

	// experience and upgrade
	int exp;
	UpgradeState health_upgrade;
	UpgradeState ammo_upgrade;

	MapState changeMap(std::string map_name);

	StateManager()
	{
		map_index = 0;
		saved_map_index = 0;
		map_lists = {"tutorial", "map1", "map2", "map3", "map4", "map5", "map6"};
		//map_lists = {"map6" };
		changeMap("test");

		exp = 0;
		health_upgrade = { 0, 5, 10 };
		ammo_upgrade = { 0, 10, 10 };

		saved_max_health = 0;
		saved_health = 0;
		saved_ammo = 0;

		saved_ammo_pack = 0;
		saved_health_potion = 0;

		// power ups
		saved_speed_boost = 0;
		saved_shield = 0;
		saved_life_steal = 0;
		saved_ricochet = 0;
		saved_attack_speed = 0;

		saved_profession = "Soldier";
	}

	~StateManager() {
		std::cout << "state destroyed" << std::endl;
	}

	void save();
	void load();
	bool is_blocked(int id);
	bool is_floor(int id);
	bool is_wall(int id);
};

extern StateManager state;