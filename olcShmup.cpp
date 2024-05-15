//// olcShmup.cpp : This file contains the 'main' function. Program execution begins and ends there.
////
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "Screen.h"
#include <unordered_map>


enum class GameState {
    MENU,
    INTRO,
    GAME,
    GAME_OVER
};

class Shmup : public olc::PixelGameEngine {
public:
    Shmup() {
        sAppName = "Shmup";
    }


    MenuScreen* menuScreen;
    IntroScreen* introScreen;
    GameScreen* gameScreen;
    Screen* currentScreen;
    std::string currentScreenStr;
    std::unordered_map<std::string, Screen*> screenMap;
    GameState gameState;

public:



    bool OnUserCreate() override {
        menuScreen = new MenuScreen(*this);
        menuScreen->Create();
        introScreen = new IntroScreen(*this);
        introScreen->Create();
        gameScreen = new GameScreen(*this);
        gameScreen->Create();
        screenMap["menu"] = menuScreen;
        screenMap["intro"] = introScreen;
        screenMap["game"] = menuScreen;

        currentScreen = gameScreen;
        currentScreenStr = "menu";
        gameState = GameState::GAME;

        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {
        switch (gameState) {
        case GameState::MENU:
            currentScreen = menuScreen;
            break;
        case GameState::INTRO:
            currentScreen = introScreen;
            break;
        case GameState::GAME:
            currentScreen = gameScreen;
            break;
        case GameState::GAME_OVER:
            // Handle game over screen
            break;
        }
        
        if (!currentScreen->Run(fElapsedTime)) {
            // Handle screen transition logic here
            switch (gameState) {
            case GameState::MENU:
                if (menuScreen->currentSelection == 0) {
                    // Start game
                    gameState = GameState::INTRO;
                }
                else if (menuScreen->currentSelection == menuScreen->lines.size() - 1) {
                    // Exit game
                    return false;
                }
                break;
            case GameState::INTRO:
                // Game over logic
                gameState = GameState::GAME;
                break;
            case GameState::GAME:
                // Game over logic
                gameState = GameState::GAME_OVER;
                break;
            case GameState::GAME_OVER:
                // Reset game or exit to menu
                gameState = GameState::MENU;
                break;
            }
        }

        return true;
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