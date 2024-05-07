//// olcShmup.cpp : This file contains the 'main' function. Program execution begins and ends there.
////
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "Player.h"

constexpr double PI = 3.14159f;

class Shmup : public olc::PixelGameEngine {
public:
    Shmup() {
        sAppName = "Shmup";
    }


    std::array<olc::Sprite*, 3> sprEnemy;

    float fWorldSpeed = 40.0f;
    double dWorldPos = 0;

    std::array<olc::vf2d, 800> arrStars;

    struct sEnemy;
    
    struct sBullet {
        olc::vf2d pos;
        olc::vf2d vel;
        bool remove = false;
    };

    struct sEnemyDefiniton {
        double dTriggerTime = 0.0f;
        uint32_t nSpriteID = 0;
        float fHealth = 0.0f;
        float fOffset = 0.0f;

        std::function<void(sEnemy&, float, float)> funcMove;
        std::function<void(sEnemy&, float, float, std::list<sBullet>&)> funcFire;

    };

    struct sEnemy {
        olc::vf2d pos;
        sEnemyDefiniton def;

        std::array<float, 4> dataMove{0};
        std::array<float, 4> dataFire{0};

        void Update(float fElpasedTime, float fScrollSpeed, std::list<sBullet> &b) {
            def.funcMove(*this, fElpasedTime, fScrollSpeed);
            def.funcFire(*this, fElpasedTime, fScrollSpeed, b);
        }
    };
    Player player;
    bool bCanFire = false;
    std::list<sEnemyDefiniton> listSpawns;
    std::list<sEnemy> listEnemies;
    std::list<sBullet> listBullets;
    std::list<sBullet> listPlayerBullets;
    std::list<sBullet> listFragments;

public:
    bool OnUserCreate() override {
        sprEnemy[0] = new olc::Sprite("assets//enemyShip01.png");
        sprEnemy[1] = new olc::Sprite("assets//enemyShip01.png");
        sprEnemy[2] = new olc::Sprite("assets//enemyShip01.png");
        

        for (auto& star : arrStars) star = {(float)(rand() % ScreenWidth()), (float)(rand() % ScreenHeight())};

        // Movement Patterns
        auto Move_None = [&](sEnemy& e, float fElapsedTime, float fScrollSpeed) {
            e.pos.y += fScrollSpeed * fElapsedTime;
        };

        auto Move_Fast = [&](sEnemy& e, float fElapsedTime, float fScrollSpeed) {
            e.pos.y += fScrollSpeed * fElapsedTime * 3.0f;
            
        };

        auto Move_SinusoidNarrow = [&](sEnemy& e, float fElapsedTime, float fScrollSpeed) {
            e.pos.y += fScrollSpeed * fElapsedTime * 1.0f;
            e.dataMove[0] += fElapsedTime;
            e.pos.x += 50.0f * cosf(e.dataMove[0]) * fElapsedTime;
        };

        auto Move_SinusoidWide = [&](sEnemy& e, float fElapsedTime, float fScrollSpeed) {
            e.pos.y += fScrollSpeed * fElapsedTime * 1.0f;
            e.dataMove[0] += fElapsedTime;
            e.pos.x += 200.0f * cosf(e.dataMove[0]) * fElapsedTime;
        };

        auto Fire_None = [&](sEnemy& e, float fElapsedTime, float fScrollSpeed, std::list<sBullet>& b) {};

        auto Fire_Straigt2 = [&](sEnemy& e, float fElapsedTime, float fScrollSpeed, std::list<sBullet>& b) {
            constexpr float fDelay = 0.2f;
            e.dataFire[0] += fElapsedTime;
            if (e.dataFire[0] >= fDelay) {
                e.dataFire[0] -= fDelay;
                sBullet b;
                b.pos = e.pos + (olc::vf2d(24, 48)); // Remove magic numbers!
                b.vel = { 0.0f, 180.0f };
                listBullets.push_back(b);
            }
        };

        auto Fire_CirclePulse2 = [&](sEnemy& e, float fElapsedTime, float fScrollSpeed, std::list<sBullet>& b) {
            constexpr float fDelay = 2.0f;
            constexpr int nBullets = 10;
            constexpr float fTetha = PI * 2.0f / ((float)nBullets);

            e.dataFire[0] += fElapsedTime;
            if (e.dataFire[0] >= fDelay) {
                e.dataFire[0] -= fDelay;
                for (int i = 0; i < nBullets; ++i) {
                    sBullet b;
                    b.pos = e.pos + (olc::vf2d(24, 24)); // Remove magic numbers!
                    b.vel = { 180.0f * cosf(fTetha * i), 180.0f * sinf(fTetha * i) };
                    listBullets.push_back(b);
                }
            }
        };

        auto Fire_DeathSpiral = [&](sEnemy& e, float fElapsedTime, float fScrollSpeed, std::list<sBullet>& b) {
            constexpr float fDelay = 0.01f;
            constexpr int nBullets = 50;
            constexpr float fTetha = PI * 2.0f / ((float)nBullets);

            e.dataFire[0] += fElapsedTime;
            if (e.dataFire[0] >= fDelay) {
                e.dataFire[0] -= fDelay;
                e.dataFire[1] += 0.1f;

                sBullet b;
                b.pos = e.pos + (olc::vf2d(24, 24)); // Remove magic numbers!
                b.vel = { 180.0f * cosf(e.dataFire[1]), 180.0f * sinf(e.dataFire[1])};
                listBullets.push_back(b);
            }
        };



        listSpawns = {
            {60.00, 0, 3.0f, 0.5f, Move_SinusoidWide, Fire_CirclePulse2},
            {240.0, 0, 3.0f, 0.25f, Move_SinusoidNarrow, Fire_Straigt2},
            {240.0, 0, 3.0f, 0.75f, Move_SinusoidNarrow, Fire_Straigt2},
            {360.0, 0, 3.0f, 0.2f, Move_None, Fire_Straigt2},
            {360.0, 0, 3.0f, 0.5f, Move_None, Fire_None},
            {360.0, 0, 3.0f, 0.8f, Move_None, Fire_Straigt2},
            {500, 0, 3.0f, 0.5f, Move_Fast, Fire_DeathSpiral}
        };

        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {
        Clear(olc::BLACK);
        for (size_t i = 0; i < arrStars.size(); ++i) {
            auto& star = arrStars[i];

            star.y += fWorldSpeed * fElapsedTime * ((i < 200) ? 0.8f : 1.0f);
            if (star.y > (float)ScreenHeight()) {
                star = { (float)(rand() % ScreenWidth()), 0.0f };
            }
            Draw(star, ((i < 200) ? olc::DARK_GREY : olc::WHITE));
        }
        
        player.UpdatePosition(fElapsedTime);

        SetPixelMode(olc::Pixel::MASK);
        player.Draw();
        
       
        SetPixelMode(olc::Pixel::NORMAL);

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


// Optimize stars to initialization
// add sprite stars/nebulas etc
// add main menu
// convert input handling to command design pattern
// make player ship not go out of bounds
// make the dataMove and dataFire as a calss member
// Remove magic numbers!
// add explosion sprite
// add score
// particle fade away
// add player death