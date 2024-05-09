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
	void Update(float);
	void Draw();
	float getWidth();
	float getHeight();
	olc::vf2d pos;
	float speed;
	float health;
	olc::Sprite* sprPlayerSheet;
	float fGunReloadTimer;
	float fGunReloadDelay;
	bool bCanFire;
	std::list<sBullet> listPlayerBullets;
	std::vector<std::tuple<Command*, float>> listPlayerCommands;
	InputHandler *ih;
	float fGraphicTimer;
	int graphicCounter;
	enum { NORTH = 0, EAST = 1, WEST = 2} facingDirection;
	enum { STANDING, MOVING, DEAD } graphicState;
private:
	float fWidth;
	float fHeight;
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
