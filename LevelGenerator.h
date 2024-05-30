#pragma once
#include "Spawn.h"

constexpr float PI = 3.14159f;

class LevelGenerator
{
public:
	LevelGenerator() {};
	void init(int32_t screenWidth, int32_t screenHeight, float enemyHealth, float bossHealth);
	void lvlOneGeneate(const std::vector<std::unique_ptr<olc::Sprite>>& listSprites, std::list<Spawn*>& listSpawns);

private:
	int screenWidth;
	int screenHeight;
	int enemyHealth;
	int bossHealth;
	std::function<void(sSpawn&, float, float)> Move_Bounce;
	std::function<void(sSpawn&, float, float)> Move_None;
	std::function<void(sSpawn&, float, float)> Move_Slow;
	std::function<void(sSpawn&, float, float)> Move_Fast;
	std::function<void(sSpawn&, float, float)> Move_SinusoidNarrow;
	std::function<void(sSpawn&, float, float)> Move_SinusoidWide;
	std::function<void(sSpawn&, float, float)> Move_SinusoidWideInf;

	std::function<void(sSpawn&, float, float, std::list<Bullet>&)> Fire_DeathSpiral;
	std::function<void(sSpawn&, float, float, std::list<Bullet>&)> Fire_None;
	std::function<void(sSpawn&, float, float, std::list<Bullet>&)> Fire_Straigt2;
	std::function<void(sSpawn&, float, float, std::list<Bullet>&)> Fire_CirclePulse2;
};

