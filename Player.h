#ifndef PLAYER_H
#define PLAYER_H
#include "olcPixelGameEngine.h"
#include "Bullet.h"

#pragma once

class InputHandler;
class Command;
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
	std::vector<std::tuple<Command*, float>> listPlayerCommands;
	InputHandler *ih;

private:
};

class Command
{
private:
	char key;
public:
	Command(char key) : key(key) {}
	virtual ~Command() {}
	virtual void execute(Player &, float) = 0;
	char getKey() const { return key; };
};

class InputHandler
{
public:
	InputHandler(olc::PixelGameEngine& pge) : pge(pge) {}
	Command* handleInput();
	olc::PixelGameEngine& pge;
};

#endif // PLAYER_H
