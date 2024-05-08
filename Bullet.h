#ifndef BULLET_H
#define BULLET_H
#pragma once
#include "olcPixelGameEngine.h"

struct sBullet {
	olc::vf2d pos;
	olc::vf2d vel;
	bool remove = false;
	uint16_t alpha;
};

#endif // BULLET_H
