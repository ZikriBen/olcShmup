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
	float getWidth() { return fWidth; };
	float getHeight() { return fHeight; };
	float getHealth() { return health; };
	void setHealth(float newHealth) { health = newHealth; };
	float getSpeed() { return speed; };

	olc::vf2d pos;
	olc::Sprite* sprPlayerSheet;
	float fGunReloadTimer;
	float fGunReloadDelay;
	bool bCanFire;
	std::list<Bullet> listPlayerBullets;
	std::vector<std::tuple<Command*, float>> listPlayerCommands;
	float fGraphicTimer;
	int graphicCounter;
	enum { STANDING, MOVING, DEAD } graphicState;
	enum { NORTH = 0, EAST = 1, WEST = 2} facingDirection;
	
private:
	float health;
	float speed;
	InputHandler *ih;
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
