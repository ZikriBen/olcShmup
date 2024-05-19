#pragma once
#include "Player.h"
#include "olcPixelGameEngine.h"
//Johnngy63

class Player;

class PlayerMovement
{

private:

	void ManageSpaceKey(float fElapsedTime);
	
public:
	PlayerMovement(olc::PixelGameEngine& pge, Player& player);
	olc::PixelGameEngine& pge;
	Player& player;


	/// <summary>
	/// Call this method to update the player poistion
	/// </summary>
	/// <param name="fElapsedTime">fElapsedTime</param>
	void Update(float fElapsedTime);

};
