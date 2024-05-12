#ifndef COMMAND_H
#define COMMAND_H

#include <unordered_map>
#include <functional>
#include "Player.h"

#pragma once
class Command
{
private:
	char key;
public:
	Command(char key) : key(key) {}
	virtual ~Command() {}
	virtual void execute(Player&, float) = 0;
	char getKey() const { return key; };
};

class buttonW_ : public Command
{
public:
	buttonW_(char key) : Command(key) {}
	virtual void execute(Player& player, float fElapsedTime)
	{
		player.graphicState = Player::MOVING;
		player.facingDirection = Player::NORTH;
		player.pos.y -= player.getSpeed() * fElapsedTime;
	}
};

class buttonS_ : public Command
{
public:
	buttonS_(char key) : Command(key) {}
	virtual void execute(Player& player, float fElapsedTime)
	{
		player.graphicState = Player::MOVING;
		player.facingDirection = Player::NORTH;
		player.pos.y += player.getSpeed() * fElapsedTime;
	}
};

class buttonA_ : public Command
{
public:
	buttonA_(char key) : Command(key) {}
	virtual void execute(Player& player, float fElapsedTime)
	{
		player.graphicState = Player::MOVING;
		player.facingDirection = Player::WEST;
		player.pos.x -= player.getSpeed() * fElapsedTime;

	}
};

class buttonD_ : public Command
{
public:
	buttonD_(char key) : Command(key) {}
	virtual void execute(Player& player, float fElapsedTime)
	{
		player.graphicState = Player::MOVING;
		player.facingDirection = Player::EAST;
		player.pos.x += player.getSpeed() * fElapsedTime;
	}
};

class buttonSPACE_ : public Command
{
public:
	buttonSPACE_(char key) : Command(key) {}
	virtual void execute(Player& player, float fElapsedTime)
	{
		if (player.bCanFire) {
			Bullet b;
			b.pos = { player.pos.x + ((float)player.getWidth() / 2.0f), player.pos.y };
			b.vel = { 0.0f, -200.0f };
			player.listPlayerBullets.push_back(b);
			player.bCanFire = false;
		}
	}
};

class buttonQ_ : public Command
{
public:
	buttonQ_(char key) : Command(key) {}
	virtual void execute(Player& player, float fElapsedTime)
	{
		for (const auto& tuple : player.listPlayerCommands) {
			Command* commandPtr = std::get<0>(tuple);
			float time = std::get<1>(tuple);
			char key = commandPtr->getKey();
			std::cout << "Command key: " << commandPtr->getKey() << "time:" << time << std::endl;
		}
	}
};

class NoCommand : public Command
{
public:
	NoCommand(char key) : Command(key) {}
	virtual void execute(Player& player, float fElapsedTime)
	{
		player.graphicState = Player::STANDING;
		player.facingDirection = Player::NORTH;
	}
};

class CommandFactory {
public:
    virtual Command* create(char key) = 0;
};

class ConcreteCommandFactory : public CommandFactory {
public:
    ConcreteCommandFactory() {
        // Initialize the hashmap with mappings between keys and constructor functions
        createFunctions['W'] = []() { return new buttonW_('W'); };
        createFunctions['S'] = []() { return new buttonS_('S'); };
        createFunctions['A'] = []() { return new buttonA_('A'); };
        createFunctions['D'] = []() { return new buttonD_('D'); };
		createFunctions['Q'] = []() { return new buttonD_('Q'); };
        createFunctions[' '] = []() { return new buttonSPACE_(' '); };
		createFunctions['\0'] = []() { return new NoCommand('\0'); };
    }

    virtual Command* create(char key) override {
        if (createFunctions.find(key) != createFunctions.end()) {
            return createFunctions[key]();
        }
        return nullptr; // Handle invalid key
    }

private:
    std::unordered_map<char, std::function<Command* ()>> createFunctions;
};


class InputHandler {
public:
	InputHandler(olc::PixelGameEngine& pge, CommandFactory& factory) : pge(pge), factory(factory) {}

	void handleInput(Player& player, float fElapsedTime) {
		// Clear the set of currently pressed keys
		pressedKeys.clear();

		// Check for key press events and add the pressed keys to the set
		if (pge.GetKey(olc::W).bHeld) pressedKeys.insert('W');
		if (pge.GetKey(olc::S).bHeld) pressedKeys.insert('S');
		if (pge.GetKey(olc::A).bHeld) pressedKeys.insert('A');
		if (pge.GetKey(olc::D).bHeld) pressedKeys.insert('D');
		if (pge.GetKey(olc::Q).bHeld) pressedKeys.insert('Q');
		if (pge.GetKey(olc::SPACE).bHeld) pressedKeys.insert(' ');

		if (pressedKeys.empty()) {
			Command* command = factory.create('\0');
			command->execute(player, fElapsedTime);
			delete command;
		}
		else {
			for (char key : pressedKeys) {
				Command* command = factory.create(key);
				command->execute(player, fElapsedTime);
				delete command;
			}
		}
	}

	// Check if a specific key is currently pressed
	bool isKeyPressed(char key) const {
		return pressedKeys.count(key) > 0;
	}

private:
	olc::PixelGameEngine& pge;
	CommandFactory& factory;
	std::unordered_set<char> pressedKeys; // Set to store currently pressed keys
};

#endif //COMMAND_H