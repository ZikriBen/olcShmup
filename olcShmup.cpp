//// olcShmup.cpp : This file contains the 'main' function. Program execution begins and ends there.
////
#define OLC_PGEX_MINIAUDIO          // Johnngy63: Added to support cool sounds
#include "olcPGEX_MiniAudio.h"

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "Screen.h"
#include <unordered_map>

enum class GameState {
    START,
    MENU,
    INTRO,
    GAME,
    GAME_OVER,
    EXIT
};

class Shmup : public olc::PixelGameEngine {
public:
    Shmup() {
        sAppName = "Shmup";
    }

    StartScreen* startScreen;
    MenuScreen* menuScreen;
    IntroScreen* introScreen;
    GameScreen* gameScreen;
    GameOverScreen* gameOverScreen;
    Screen* currentScreen;
    std::string currentScreenStr;
    std::unordered_map<std::string, Screen*> screenMap;
    GameState gameState;


public:
    bool OnUserCreate() override {
        startScreen = new StartScreen(*this);
        startScreen->Create();
        menuScreen = new MenuScreen(*this);
        menuScreen->Create();
        introScreen = new IntroScreen(*this);
        introScreen->Create();
        gameScreen = new GameScreen(*this);
        gameScreen->Create();
        gameOverScreen = new GameOverScreen(*this);
        //gameOverScreen->Create();
        screenMap["start"] = startScreen;
        screenMap["menu"] = menuScreen;
        screenMap["intro"] = introScreen;
        screenMap["game"] = gameScreen;
        screenMap["game_over"] = gameOverScreen;

        gameState = GameState::GAME;
       
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {
        currentScreen = screenMap[EnumStateToString(gameState)];

        bool keepState = currentScreen->Run(fElapsedTime);
        
        if (!keepState) 
            stateTransition(gameState);

        return (gameState == GameState::EXIT) ? false : true;
    }

    std::string EnumStateToString(GameState state) {
        std::string screen = "";
        switch (gameState) {
        case GameState::START:
            screen = "start";
            break;
        case GameState::MENU:
            screen = "menu";
            break;
        case GameState::INTRO:
            screen = "intro";
            break;
        case GameState::GAME:
            screen = "game";
            break;
        case GameState::GAME_OVER:
            screen = "game_over";
            break;
        }

        return screen;
    }

    void stateTransition(GameState state) {
        // Handle screen transition logic here
        switch (gameState) {
        case GameState::START:
            gameState = GameState::MENU;
            break;
        case GameState::MENU:
            if (menuScreen->currentSelection == 0) {
                gameState = GameState::INTRO;
            }
            else if (menuScreen->currentSelection == menuScreen->lines.size() - 1) {
                gameState = GameState::EXIT;
            }
            break;
        case GameState::INTRO:
            gameScreen->Create();
            gameState = GameState::GAME;
            break;
        case GameState::GAME:
            gameScreen->Destroy();
            gameState = GameState::GAME_OVER;
            break;
        case GameState::GAME_OVER:
            gameOverScreen->Create();
            gameState = GameState::MENU;
            gameOverScreen->Destroy();
            break;
        }
    }
};


int main()
{
    Shmup game;
    if (game.Construct(640, 480, 2, 2))
        game.Start();

    return 0;
}

// 2. add score
