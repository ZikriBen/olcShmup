//// olcShmup.cpp : This file contains the 'main' function. Program execution begins and ends there.
////
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"

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

    Player player{ *this };
    std::list<sEnemyDefiniton> listSpawns;
    std::list<sEnemy> listEnemies;
    std::list<sBullet> listBullets;
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

    void detectPlayerBulletCollision(float fElapsedTime, std::list<sBullet>& playerBullets, std::list<sEnemy>& listEnemies, std::list<sBullet>* listFragments) {
        for (auto& b : playerBullets) {
            b.pos += (b.vel + olc::vd2d(0.0f, fWorldSpeed)) * fElapsedTime;
            for (auto& e : listEnemies) 
                if ((b.pos - (e.pos + olc::vd2d(24, 24))).mag2() < 24.0f * 24.0f) { // Remove magic numbers!
                    b.remove = true;
                    e.def.fHealth -= 1.0f;
                    if (e.def.fHealth <= 0) 
                        for (int i = 0; i < 500; ++i) {
                            float fAngle = ((float)rand() / (float)RAND_MAX) * 2.0f * PI;
                            float fSpeed = ((float)rand() / (float)RAND_MAX) * 200.0f + 50.0f;
                            listFragments->push_back({ e.pos + olc::vf2d(24,24), {fSpeed * cosf(fAngle), fSpeed * sinf(fAngle)} });
                        }
                }
        }
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
        
        dWorldPos += fWorldSpeed * fElapsedTime;
        player.UpdatePosition(fElapsedTime);


        while (!listSpawns.empty() && dWorldPos >= listSpawns.front().dTriggerTime) {
            sEnemy e;
            e.def = listSpawns.front();
            e.pos = {
                e.def.fOffset * ((float)ScreenWidth()) - (((float)sprEnemy[e.def.nSpriteID]->width) / 2),
                0.0f - ((float)sprEnemy[e.def.nSpriteID]->height)
            };
            listSpawns.pop_front();
            listEnemies.push_back(e);
        }

        for (auto& b : listFragments) {
            b.pos += (b.vel + olc::vd2d(0.0f, fWorldSpeed)) * fElapsedTime;
        }

        for (auto& e : listEnemies) e.Update(fElapsedTime, fWorldSpeed, listBullets);

        for (auto& b : listBullets) {
            b.pos += (b.vel + olc::vd2d(0.0f, fWorldSpeed)) * fElapsedTime;
            if ((b.pos - (player.pos + olc::vd2d(24, 24))).mag2() < 24.0f * 24.0f) { // Remove magic numbers!
                b.remove = true;
                player.health -= 1.0f;
            }
        }

        detectPlayerBulletCollision(fElapsedTime, player.listPlayerBullets, listEnemies, &listFragments);

        listEnemies.remove_if([&](const sEnemy& e) {return e.pos.y >= ((float)ScreenHeight()) || e.def.fHealth <= 0; });

        listBullets.remove_if([&](const sBullet& b) {return b.pos.x < 0 || b.pos.x>ScreenWidth() || b.pos.y <0 || b.pos.y>ScreenHeight() || b.remove; });

        listFragments.remove_if([&](const sBullet& b) {return b.pos.x<0 || b.pos.x>ScreenWidth() || b.pos.y <0 || b.pos.y>ScreenHeight() || b.remove; });

        SetPixelMode(olc::Pixel::MASK);
        
        //DRAW
        player.Draw();
        
        for (auto& e : listEnemies) DrawSprite(e.pos, sprEnemy[e.def.nSpriteID], 1, olc::Sprite::Flip::VERT);

        for (auto& b : listBullets) FillCircle(b.pos, 3, olc::RED);

        for (auto& b : player.listPlayerBullets) FillCircle(b.pos, 3, olc::CYAN);
        for (auto& b : listFragments) Draw(b.pos, olc::YELLOW);

        //HUD
        DrawString(4, 4, "HEALTH:");
        FillRect(60, 4, (player.health / 100.0f * 576.0f), 8, olc::GREEN); // Remove magic numbers!
       
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

// make oop design
// convert input handling to command design pattern
// Optimize stars to initialization
// add sprite stars/nebulas etc
// add main menu
// make the dataMove and dataFire as a calss member
// Remove magic numbers!
// add explosion sprite
// add score
// particle fade away
// add player death