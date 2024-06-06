#pragma once
#include "Player.h"
#include "olcPixelGameEngine.h"
//Johnngy63

class Player;

class PlayerMovement
{

private:
	bool bEnable = true;
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
	void setEnable(bool isEnable) { bEnable = isEnable; }
};
