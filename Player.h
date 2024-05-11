#ifndef PLAYER_H
#define PLAYER_H
#include "olcPixelGameEngine.h"
#include <unordered_set>
#include "Bullet.h"

#pragma once

class InputHandler;
class Command;
class CommandFactory;

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
	void kill() { dead = false; };
	bool isDead() { return dead; };

	olc::vf2d pos;
	olc::Sprite* sprPlayerSheet;
	float fGunReloadTimer;
	float fGunReloadDelay;
	bool bCanFire;
	std::list<Bullet> listPlayerBullets;
	std::vector<std::tuple<Command*, float>> listPlayerCommands;
	float fGraphicTimer;
	int graphicCounter;
	CommandFactory* factory;
	enum { STANDING, MOVING } graphicState;
	enum { ALIVE, DYING, DEAD } lifeState;
	enum { NORTH = 0, EAST = 1, WEST = 2} facingDirection;
	
private:
	bool dead;
	float health;
	float speed;
	InputHandler *ih;
	float fWidth;
	float fHeight;
};



#endif // PLAYER_H
