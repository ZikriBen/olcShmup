#ifndef BULLET_H
#define BULLET_H
#include "olcPixelGameEngine.h"

#pragma once
class Player;
class Bullet {
public:
	olc::vf2d pos;
	olc::vf2d vel;
	bool remove = false;
	uint16_t alpha;
	enum { DEFAULT = 0, GREEN = 1, BLUE = 2 } bulletType;
	void Update(const float fElapsedTime, const float fWorldSpeed, Player& player);
};

#endif // BULLET_H
