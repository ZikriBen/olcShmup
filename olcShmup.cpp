//// olcShmup.cpp : This file contains the 'main' function. Program execution begins and ends there.
////
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "Screen.h"



class Shmup : public olc::PixelGameEngine {
public:
    Shmup() {
        sAppName = "Shmup";
    }

    
    MenuScreen *menuScreen;
    GameScreen* gameScreen;
    Screen *currentScreen;
    

public:



    bool OnUserCreate() override {
        menuScreen = new MenuScreen(*this);
        menuScreen->Create();
        gameScreen = new GameScreen(*this);
        gameScreen->Create();
        
        
        currentScreen = gameScreen;
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {
        return currentScreen->Run(fElapsedTime);
        /*if (!gameStart) {
            
            return true;
        }
        else {
            GameUpdate(fElapsedTime);
        }*/
    }


};



int main()
{
	Shmup game;
	if (game.Construct(640, 480, 2, 2))
		game.Start();

	return 0;
}

// 1. add boss
// 2. add score
// 3. add main menu