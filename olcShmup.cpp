//// olcShmup.cpp : This file contains the 'main' function. Program execution begins and ends there.
////
#include <memory>
#define OLC_PGEX_MINIAUDIO          // Johnngy63: Added to support cool sounds
#include "olcPGEX_MiniAudio.h"

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "Screen.h"


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

    std::unique_ptr<StartScreen> startScreen;
    std::unique_ptr<MenuScreen> menuScreen;
    std::unique_ptr<IntroScreen> introScreen;
    std::unique_ptr<GameScreen> gameScreen;
    std::unique_ptr<GameOverScreen> gameOverScreen;
    Screen* currentScreen;
    std::string currentScreenStr;
    std::unordered_map<std::string, Screen*> screenMap;
    GameState gameState;


public:
    bool OnUserCreate() override {
        startScreen = std::make_unique<StartScreen>(*this);
        startScreen->Create();
        menuScreen = std::make_unique<MenuScreen>(*this);
        menuScreen->Create();
        introScreen = std::make_unique<IntroScreen>(*this);
        introScreen->Create();
        gameScreen = std::make_unique<GameScreen>(*this);
        gameScreen->Create();
        gameOverScreen = std::make_unique<GameOverScreen>(*this);
        gameOverScreen->Create();

        screenMap["start"] = startScreen.get();
        screenMap["menu"] = menuScreen.get();
        screenMap["intro"] = introScreen.get();
        screenMap["game"] = gameScreen.get();
        screenMap["game_over"] = gameOverScreen.get();

        gameState = GameState::GAME;

        // For Debug mode
        if (gameState == GameState::GAME) {
            gameScreen->Reset();
            gameScreen->CreateSpawns();
        }

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
        case GameState::EXIT:
            break;
        }

        return screen;
    }

    void stateTransition(GameState state) {
        // Handle screen transition logic here
        switch (gameState) {
        case GameState::START:
            gameState = GameState::MENU;
            menuScreen->Reset();
            startScreen->Destroy(); // Only Destroy, as no need for start screen anymore
            break;
        case GameState::MENU:
            if (menuScreen->currentSelection == 0) {
                gameState = GameState::INTRO;
            }
            else if (menuScreen->currentSelection == menuScreen->lines.size() - 1) {
                gameState = GameState::EXIT;
            }
            introScreen->Reset();
            break;
        case GameState::INTRO:
            gameScreen->Reset();
            gameScreen->CreateSpawns();
            gameState = GameState::GAME;
            break;
        case GameState::GAME:
            gameOverScreen->Reset();
            gameState = GameState::GAME_OVER;
            gameScreen->Destroy();
            break;
        case GameState::GAME_OVER:
            menuScreen->Reset();
            gameState = GameState::MENU;
            break;
        case GameState::EXIT:
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

// 1. Add HUD