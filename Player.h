#ifndef PLAYER_H
#define PLAYER_H
#include "olcPixelGameEngine.h"

#pragma once
struct sBullet {
	olc::vf2d pos;
	olc::vf2d vel;
	bool remove = false;
};

class Player
{
public:
	Player(olc::PixelGameEngine& pge);
	olc::PixelGameEngine& pge;
	void UpdatePosition(float);
	void Draw();
	olc::vf2d pos;
	float speed;
	float health;
	olc::Sprite* sprPlayer;
	float fGunReloadTimer;
	float fGunReloadDelay;
	bool bCanFire;
	std::list<sBullet> listPlayerBullets;
private:
};

#endif // PLAYER_H
