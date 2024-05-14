//// olcShmup.cpp : This file contains the 'main' function. Program execution begins and ends there.
////
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "Screen.h"
#include <unordered_map>


class Shmup : public olc::PixelGameEngine {
public:
    Shmup() {
        sAppName = "Shmup";
    }

    
    MenuScreen *menuScreen;
    GameScreen* gameScreen;
    Screen *currentScreen;
    std::string currentScreenStr;
    std::unordered_map<std::string, Screen*> screenMap;

public:



    bool OnUserCreate() override {
        menuScreen = new MenuScreen(*this);
        menuScreen->Create();
        gameScreen = new GameScreen(*this);
        gameScreen->Create();
        screenMap["menu"] = menuScreen;
        screenMap["game"] = menuScreen;

        currentScreen = gameScreen;
        currentScreenStr = "menu";

        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {
        return screenMap[currentScreenStr]->Run(fElapsedTime);
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