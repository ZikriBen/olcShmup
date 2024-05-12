#ifndef EXPLOSION_H
#define EXPLOSION_H
#include "olcPixelGameEngine.h"
#include "Player.h"

#pragma once
class Explosion
{
public:
	Explosion(olc::PixelGameEngine& pge, olc::vf2d pos, olc::Sprite* spriteSheet) : pge(pge), pos(pos), spriteSheet(spriteSheet) {};
	Explosion(olc::PixelGameEngine& pge) : pge(pge) {};
	olc::PixelGameEngine& pge;
	olc::Sprite* spriteSheet;
	olc::vf2d pos;
	float fGraphicTimer = 0.0f;
	int graphicCounter = 0;
	int frames = 7;
	int width = 80;
	int height = 80;
	float fDelay = 0.0;
	bool remove = false;
	void Update(float fElapsedTime, Player&);
	void Draw();
	void Start();
};

#endif //EXPLOSION_H