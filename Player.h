#ifndef PLAYER_H
#define PLAYER_H
#include "olcPGEX_MiniAudio.h"
#include "olcPixelGameEngine.h"

// Jonngy63: Added PlayerMovement.h
#include "PlayerMovement.h";
#include <unordered_set>
#include "Bullet.h"

#pragma once

class InputHandler;

// Johhnyg63: Removed PlayerMovement now handles this
//class Command;
//class CommandFactory;

class ProjectileDef {
public:
	ProjectileDef() : spr(nullptr), type(0) {};
	ProjectileDef(olc::Sprite* spr, int type) : spr(spr), type(type) {};
	olc::Sprite* spr;
	int type = Bullet::DEFAULT;
	int offsetX = 0;
	int offsetY = 0;
	float fireRate = 0.2f;
};

class Player
{
public:
	Player(olc::PixelGameEngine& pge, olc::MiniAudio& miniAudio);
	olc::PixelGameEngine& pge;
	olc::MiniAudio& miniAudio; // Johnnyg63: We need to inject the miniAudio so we can play the laser sounds when the player is shooting
	void Update(float);
	void Draw();
	float getWidth() { return fWidth; };
	float getHeight() { return fHeight; };
	float getHealth() { return health; };
	void reset();
	void setHealth(float newHealth) { health = newHealth; };
	void setPoerUpLeve(int change) { powerUpLevel = std::max(1, std::min(powerUpCap, powerUpLevel + change)); };
	float getSpeed() { return speed; };
	void kill() { dead = false; };
	bool isDead() { return dead; };
	int ProjectileType = 1;
	int powerUpLevel = 1;
	olc::vf2d pos;
	olc::Sprite* sprPlayerSheet;
	float fGunReloadTimer;
	float fGunReloadDelay;
	bool bCanFire;
	std::list<Bullet> listPlayerBullets;
	std::vector<ProjectileDef> listProjectileDef;
	//std::vector<std::tuple<Command*, float>> listPlayerCommands;
	float fGraphicTimer;
	int graphicCounter;
	//CommandFactory* factory;
	enum { STANDING, MOVING } graphicState;
	enum { ALIVE, DYING, DEAD } lifeState;
	enum { NORTH = 0, EAST = 1, WEST = 2} facingDirection;

	// Johnnyg63: Lasers sounds for when the user is shooting (Space bar)
	std::string souLaserA1 = "assets\\sounds\\laser_a1.mp3"; // Holds the full path to laser_a1.mp3
	
private:
	int powerUpCap = 3;
	bool dead;
	float health;
	float speed;
	InputHandler *ih;
	float fWidth;
	float fHeight;
};

#endif // PLAYER_H
