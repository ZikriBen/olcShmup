#ifndef PLAYER_H
#define PLAYER_H
#include "olcPixelGameEngine.h"
#include "Bullet.h"
#include <unordered_set>

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

class InputHandler {
public:
	InputHandler(olc::PixelGameEngine& pge) : pge(pge) {}

	void handleInput() {
		// Clear the set of currently pressed keys
		pressedKeys.clear();

		// Check for key press events and add the pressed keys to the set
		if (pge.GetKey(olc::W).bHeld) pressedKeys.insert('W');
		if (pge.GetKey(olc::S).bHeld) pressedKeys.insert('S');
		if (pge.GetKey(olc::A).bHeld) pressedKeys.insert('A');
		if (pge.GetKey(olc::D).bHeld) pressedKeys.insert('D');
		if (pge.GetKey(olc::SPACE).bHeld) pressedKeys.insert(' ');

		// Handle other input events as needed
	}

	// Check if a specific key is currently pressed
	bool isKeyPressed(char key) const {
		return pressedKeys.count(key) > 0;
	}

private:
	olc::PixelGameEngine& pge;
	std::unordered_set<char> pressedKeys; // Set to store currently pressed keys
};

#endif // PLAYER_H
