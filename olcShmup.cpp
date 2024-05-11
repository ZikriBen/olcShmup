//// olcShmup.cpp : This file contains the 'main' function. Program execution begins and ends there.
////
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Background.h"
#include "Explosion.h"
constexpr double PI = 3.14159f;

class Shmup : public olc::PixelGameEngine {
public:
    Shmup() {
        sAppName = "Shmup";
    }

    float fWorldSpeed = 40.0f;
    double dWorldPos = 0;

    

    Player player{ *this };
    Background bg{ *this, fWorldSpeed, 200 };
    Explosion *exp = nullptr;
    std::list<sEnemyDefiniton> listSpawns;
    std::list<sEnemy> listEnemies;
    std::list<Bullet> listEnemyBullets;
    std::list<Bullet> listFragments;
    std::list<Explosion *> listExplosions;

public:
    bool OnUserCreate() override {
        player.pos = olc::vf2d(((float)ScreenWidth() / 2.0f) - player.getWidth() / 2.0f, ((float)ScreenHeight() - player.getHeight() - 50.0f));
        player.lifeState = Player::ALIVE;
        bg.populateStars();
        exp = new Explosion(*this);
        exp->spriteSheet = new olc::Sprite("assets/explosion-spritesheet2.png");
        
        
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

        auto Fire_None = [&](sEnemy& e, float fElapsedTime, float fScrollSpeed, std::list<Bullet>& b) {};

        auto Fire_Straigt2 = [&](sEnemy& e, float fElapsedTime, float fScrollSpeed, std::list<Bullet>& b) {
            constexpr float fDelay = 0.2f;
            e.dataFire[0] += fElapsedTime;
            if (e.dataFire[0] >= fDelay) {
                e.dataFire[0] -= fDelay;
                Bullet b;
                b.pos = e.pos + (olc::vf2d(((float)e.def.sprEnemy->width / 2.0f), ((float)e.def.sprEnemy->height)));
                b.vel = { 0.0f, 180.0f };
                listEnemyBullets.push_back(b);
            }
        };

        auto Fire_CirclePulse2 = [&](sEnemy& e, float fElapsedTime, float fScrollSpeed, std::list<Bullet>& b) {
            constexpr float fDelay = 2.0f;
            constexpr int nBullets = 10;
            constexpr float fTetha = PI * 2.0f / ((float)nBullets);

            e.dataFire[0] += fElapsedTime;
            if (e.dataFire[0] >= fDelay) {
                e.dataFire[0] -= fDelay;
                for (int i = 0; i < nBullets; ++i) {
                    Bullet b;
                    b.pos = e.pos + (olc::vf2d(((float)e.def.sprEnemy->width / 2.0f), ((float)e.def.sprEnemy->height / 2.0f)));
                    b.vel = { 180.0f * cosf(fTetha * i), 180.0f * sinf(fTetha * i) };
                    listEnemyBullets.push_back(b);
                }
            }
        };

        auto Fire_DeathSpiral = [&](sEnemy& e, float fElapsedTime, float fScrollSpeed, std::list<Bullet>& b) {
            constexpr float fDelay = 0.01f;
            constexpr int nBullets = 50;
            constexpr float fTetha = PI * 2.0f / ((float)nBullets);

            e.dataFire[0] += fElapsedTime;
            if (e.dataFire[0] >= fDelay) {
                e.dataFire[0] -= fDelay;
                e.dataFire[1] += 0.1f;

                Bullet b;
                b.pos = e.pos + (olc::vf2d(24, 24)); // Remove magic numbers!
                b.vel = { 180.0f * cosf(e.dataFire[1]), 180.0f * sinf(e.dataFire[1])};
                listEnemyBullets.push_back(b);
            }
        };

        olc::Sprite *enemyShip01 = new olc::Sprite("assets/enemyShip01.png");
        
        listSpawns = {
            {60.00, enemyShip01, 3.0f, 0.5f, Move_SinusoidWide, Fire_Straigt2},
            {240.0, enemyShip01, 3.0f, 0.25f, Move_SinusoidNarrow, Fire_Straigt2},
            {240.0, enemyShip01, 3.0f, 0.75f, Move_SinusoidNarrow, Fire_Straigt2},
            {360.0, enemyShip01, 3.0f, 0.2f, Move_None, Fire_Straigt2},
            {360.0, enemyShip01, 3.0f, 0.5f, Move_None, Fire_CirclePulse2},
            {360.0, enemyShip01, 3.0f, 0.8f, Move_None, Fire_Straigt2},
            {500.0, enemyShip01, 3.0f, 0.5f, Move_Fast, Fire_DeathSpiral}
        };

        return true;
    }

    void detectPlayerBulletCollision(float fElapsedTime, std::list<Bullet>& playerBullets, std::list<sEnemy>& listEnemies, std::list<Bullet>* listFragments) {
        for (auto& b : playerBullets) {
            b.pos += (b.vel + olc::vf2d(0.0f, fWorldSpeed)) * fElapsedTime;
            for (auto& e : listEnemies) 
                if ((b.pos - (e.pos + olc::vf2d(((float)e.def.sprEnemy->width / 2.0f), ((float)e.def.sprEnemy->width / 2.0f)))).mag2() <  powf(((float)e.def.sprEnemy->width / 2.0f), 2.0f)) { // Remove magic numbers!
                    b.remove = true;
                    e.def.fHealth -= 1.0f;
                    if (e.def.fHealth <= 0)  {
                        
                        for (int i = 0; i < 500; ++i) {
                            float fAngle = ((float)rand() / (float)RAND_MAX) * 2.0f * PI;
                            float fSpeed = ((float)rand() / (float)RAND_MAX) * 200.0f + 50.0f;
                            listFragments->push_back({ e.pos + olc::vf2d(24, 24), {fSpeed * cosf(fAngle), fSpeed * sinf(fAngle)}, false, 255 });
                        }
                    }
                }
        }
    }

    void cleanUp() {
        listEnemies.remove_if([&](const sEnemy& e) {return e.pos.y >= ((float)ScreenHeight()) || e.def.fHealth <= 0; });

        listEnemyBullets.remove_if([&](const Bullet& b) {return b.pos.x < 0 || b.pos.x>ScreenWidth() || b.pos.y <0 || b.pos.y>ScreenHeight() || b.remove; });

        listFragments.remove_if([&](const Bullet& b) {return b.pos.x<0 || b.pos.x>ScreenWidth() || b.pos.y <0 || b.pos.y>ScreenHeight() || b.remove; });

        listExplosions.remove_if([&](const Explosion* e) {return e->pos.x<0 || e->pos.x>ScreenWidth() || e->pos.y <0 || e->pos.y>ScreenHeight() || e->remove; });
    }

    bool OnUserUpdate(float fElapsedTime) override {
        Clear(olc::BLACK);
        dWorldPos += fWorldSpeed * fElapsedTime;
        bg.Update(fElapsedTime);
        player.Update(fElapsedTime);
        exp->Update(fElapsedTime, player);
        
        
        
        while (!listSpawns.empty() && dWorldPos >= listSpawns.front().dTriggerTime) {
            sEnemy e;
            e.def = listSpawns.front();
            e.pos = {
                e.def.fOffset * ((float)ScreenWidth()) - (((float)e.def.sprEnemy->width) / 2),
                0.0f - ((float)e.def.sprEnemy->height)
            };
            listSpawns.pop_front();
            listEnemies.push_back(e);
        }

        for (auto& b : listFragments) {
            b.pos += (b.vel + olc::vf2d(0.0f, fWorldSpeed)) * fElapsedTime;
        }

        for (auto& e : listEnemies) {
            e.Update(fElapsedTime, fWorldSpeed, listEnemyBullets);
        }

        for (auto& b : listEnemyBullets) {
            b.Update(fElapsedTime, fWorldSpeed, player);
        }

        detectPlayerBulletCollision(fElapsedTime, player.listPlayerBullets, listEnemies, &listFragments);

        //DRAW
        bg.Draw();
        switch (player.lifeState)
        {
        case Player::ALIVE:
            player.Draw(); 
            break;
        case Player::DYING:
            exp->pos = olc::vf2d(player.pos.x, player.pos.y);
            listExplosions.push_back(exp);
            break;
        case Player::DEAD:
            break;
        default:
            break;
        }

        SetPixelMode(olc::Pixel::MASK);
        for (auto& e : listEnemies) DrawSprite(e.pos, e.def.sprEnemy, 1, olc::Sprite::Flip::VERT);
        SetPixelMode(olc::Pixel::NORMAL);

        for (auto& b : listEnemyBullets) FillCircle(b.pos, 3, olc::RED);

        for (auto& e : listExplosions) e->Draw();
        
        for (auto& b : listFragments) {
            Draw(b.pos, olc::Pixel(255, 255, 0, b.alpha));
            b.alpha -= (fElapsedTime * 0.005f);
        }

        //HUD
        DrawString(4, 4, "HEALTH:");
        FillRect(60, 4, (player.getHealth() / 100.0f * 576.0f), 8, olc::GREEN); // Remove magic numbers!
       
        cleanUp();
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

// 2. add power ups
// 3. add score
// 4. add different projectiles
// 5. add main menu