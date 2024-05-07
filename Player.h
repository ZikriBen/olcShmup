#ifndef PLAYER_H
#define PLAYER_H
#include "olcPixelGameEngine.h"

#pragma once
class Player : public olc::PixelGameEngine 
{
public:
	Player();
	void UpdatePosition(float);
	void Draw();
	olc::vf2d pos;
	float speed;
	float health;
	olc::Sprite* sprPlayer;
	float fGunReloadTimer;
	float fGunReloadDelay;
private:
};

#endif // PLAYER_H
