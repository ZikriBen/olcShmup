//// olcShmup.cpp : This file contains the 'main' function. Program execution begins and ends there.
////
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Background.h"
#include "Explosion.h"
#include "PowerUp.h"
#include "Text.h"

constexpr float PI = 3.14159f;

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
    PowerUp* con;
    std::list<Spawn *> listSpawns;
    std::list<sEnemy> listEnemies;
    std::list<Bullet> listEnemyBullets;
    std::list<Bullet> listFragments;
    std::list<Explosion *> listExplosions;
    std::list<PowerUp> listPowerUp;
    olc::Sprite *sprBG;
    olc::Decal* iconDecal;
    olc::vf2d pos;
    std::vector<Text> lines;
    int currentSelection = 0;
    bool gameStart = false;

public:

    void detectPlayerBulletCollision(float fElapsedTime, std::list<Bullet>& playerBullets, std::list<sEnemy>& listEnemies, std::list<Bullet>* listFragments) {
        for (auto& b : playerBullets) {
            b.pos += (b.vel + olc::vf2d(0.0f, fWorldSpeed)) * fElapsedTime;
            for (auto& e : listEnemies) 
                if ((b.pos - (e.pos + olc::vf2d(((float)e.def->spr->width / 2.0f), ((float)e.def->spr->width / 2.0f)))).mag2() <  powf(((float)e.def->spr->width / 2.0f), 2.0f)) { // Remove magic numbers!
                    b.remove = true;
                    e.def->fHealth -= 1.0f;
                    if (e.def->fHealth <= 0)  {
                        
                        for (int i = 0; i < 500; ++i) {
                            float fAngle = ((float)rand() / (float)RAND_MAX * 2.0f * PI);
                            float fSpeed = ((float)rand() / (float)RAND_MAX * 200.0f + 50.0f);
                            listFragments->push_back({ e.pos + olc::vf2d(24, 24), {fSpeed * cosf(fAngle), fSpeed * sinf(fAngle)}, false, 255 });
                        }
                    }
                }
        }
    }

    void detectPlayerPowerUpCollision(float fElapsedTime, Player &player, std::list<PowerUp>& listPowerUp) {
        for (auto& p : listPowerUp) {
                if ((p.pos - (player.pos + olc::vf2d(((float)player.getWidth() / 2.0f), ((float)player.getWidth() / 2.0f)))).mag2() < powf(((float)player.getWidth() / 2.0f), 2.0f)) {
                    if (p.def->type == powerUpType::DEFAULT)
                        player.setPoerUpLeve(1);
                    else if (p.def->type == powerUpType::GREEN)
                        player.ProjectileType = Bullet::GREEN;
                    else if (p.def->type == powerUpType::BLUE)
                        player.ProjectileType = Bullet::BLUE;
                    p.remove = true;
                }
        }
    }

    void cleanUp() {
        listEnemies.remove_if([&](const sEnemy& e) {return e.pos.y >= ((float)ScreenHeight()) || e.def->fHealth <= 0; });

        listEnemyBullets.remove_if([&](const Bullet& b) {return b.pos.x < 0 || b.pos.x>ScreenWidth() || b.pos.y <0 || b.pos.y>ScreenHeight() || b.remove; });

        listFragments.remove_if([&](const Bullet& b) {return b.pos.x<0 || b.pos.x>ScreenWidth() || b.pos.y <0 || b.pos.y>ScreenHeight() || b.remove; });

        listExplosions.remove_if([&](const Explosion* e) {return  e->remove; });

        listPowerUp.remove_if([&](const PowerUp& p) {return p.remove; });
    }

    bool OnUserCreate() override {
        GameCreate();
        sprBG = new olc::Sprite("assets/MainScreen640x480noText.png");
        iconDecal = new olc::Decal(new olc::Sprite("assets/iconShip.png"));
        
        lines.push_back(Text("Start Game", 316));
        lines.push_back(Text("Options", 220));
        lines.push_back(Text("About", 152));
        
        
        olc::vf2d pos = olc::vf2d(0,0);
        
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {
        if (!gameStart) {
            Clear(olc::BLACK);
            dWorldPos += fWorldSpeed * fElapsedTime;
            int mid = (ScreenWidth() / 2);
            int offsetY = (ScreenHeight() / 2 + 160);

            DrawSprite(pos, sprBG);

            SetPixelMode(olc::Pixel::NORMAL);

            olc::Pixel p = olc::WHITE;

            for (int i = 0; i < lines.size(); ++i) {
                p = i == currentSelection ? olc::YELLOW : olc::WHITE;
                DrawString(mid - (lines[i].getSize() / 4), offsetY + (i * 25), lines[i].sText, p, 2);
            }

            InputHandling();
            SetPixelMode(olc::Pixel::NORMAL);
            return true;
        }
        else {
            GameUpdate(fElapsedTime);
        }
    }


    void InputHandling() {
        if (GetKey(olc::W).bPressed || GetKey(olc::UP).bPressed) currentSelection = (currentSelection - 1 + lines.size()) % lines.size();
        if (GetKey(olc::S).bPressed || GetKey(olc::DOWN).bPressed) currentSelection = (currentSelection + 1) % lines.size();
        if (GetKey(olc::SPACE).bPressed || GetKey(olc::ENTER).bPressed) {
            if (currentSelection == 0) startGame();
            else if (currentSelection == lines.size() - 1) {
                std::cout << "ASD" << std::endl;
            }
        }
        
    }

    void startGame() {
        
        gameStart = true;
    }

    bool GameCreate() {
        player.pos = olc::vf2d(((float)ScreenWidth() / 2.0f) - player.getWidth() / 2.0f, ((float)ScreenHeight() - player.getHeight() - 50.0f));
        player.lifeState = Player::ALIVE;
        bg.populateStars();
        exp = new Explosion(*this);
        exp->spriteSheet = new olc::Sprite("assets/explosion-spritesheet2.png");


        // Movement Patterns
        auto Move_None = [&](sSpawn& e, float fElapsedTime, float fScrollSpeed) {
            e.pos.y += fScrollSpeed * fElapsedTime;
        };


        auto Move_Fast = [&](sSpawn& e, float fElapsedTime, float fScrollSpeed) {
            e.pos.y += fScrollSpeed * fElapsedTime * 3.0f;

        };

        auto Move_SinusoidNarrow = [&](sSpawn& e, float fElapsedTime, float fScrollSpeed) {
            e.pos.y += fScrollSpeed * fElapsedTime * 1.0f;
            e.dataMove[0] += fElapsedTime;
            e.pos.x += 50.0f * cosf(e.dataMove[0]) * fElapsedTime;
        };

        auto Move_SinusoidWide = [&](sSpawn& e, float fElapsedTime, float fScrollSpeed) {
            e.pos.y += fScrollSpeed * fElapsedTime * 1.0f;
            e.dataMove[0] += fElapsedTime;
            e.pos.x += 200.0f * cosf(e.dataMove[0]) * fElapsedTime;
        };

        auto Move_Bounce = [&](sSpawn& e, float fElapsedTime, float fScrollSpeed) {
            // Update position based on velocity

            e.pos += e.velocity * fElapsedTime;

            // Check collision with screen boundaries
            if (e.pos.x < 0) {
                e.pos.x = 0;
                e.velocity.x *= -1;
            }
            else if (e.pos.x + e.fWidth > ScreenWidth()) {
                e.pos.x = ((float)ScreenWidth() - e.fWidth);
                e.velocity.x *= -1;
            }

            if (e.pos.y < 0) {
                e.pos.y = 0;
                e.velocity.y *= -1;
            }
            else if (e.pos.y + e.fHeight > ScreenHeight()) {
                e.pos.y = ((float)ScreenHeight() - e.fHeight);
                e.velocity.y *= -1;
            }
        };

        auto Fire_None = [&](sSpawn& e, float fElapsedTime, float fScrollSpeed, std::list<Bullet>& b) {};

        auto Fire_Straigt2 = [&](sSpawn& s, float fElapsedTime, float fScrollSpeed, std::list<Bullet>& b) {
            sEnemy& e = *dynamic_cast<sEnemy*>(&s);
            constexpr float fDelay = 0.2f;
            e.dataFire[0] += fElapsedTime;
            if (e.dataFire[0] >= fDelay) {
                e.dataFire[0] -= fDelay;
                Bullet b;
                b.pos = e.pos + (olc::vf2d(((float)e.def->spr->width / 2.0f), ((float)e.def->spr->height)));
                b.vel = { 0.0f, 180.0f };
                listEnemyBullets.push_back(b);
            }
        };

        auto Fire_CirclePulse2 = [&](sSpawn& s, float fElapsedTime, float fScrollSpeed, std::list<Bullet>& b) {
            sEnemy& e = *dynamic_cast<sEnemy*>(&s);
            constexpr float fDelay = 2.0f;
            constexpr int nBullets = 10;
            constexpr float fTetha = ((float)PI * 2.0f / nBullets);

            e.dataFire[0] += fElapsedTime;
            if (e.dataFire[0] >= fDelay) {
                e.dataFire[0] -= fDelay;
                for (int i = 0; i < nBullets; ++i) {
                    Bullet b;
                    b.pos = e.pos + (olc::vf2d(((float)e.def->spr->width / 2.0f), ((float)e.def->spr->height / 2.0f)));
                    b.vel = { 180.0f * cosf(fTetha * i), 180.0f * sinf(fTetha * i) };
                    listEnemyBullets.push_back(b);
                }
            }
        };

        auto Fire_DeathSpiral = [&](sSpawn& s, float fElapsedTime, float fScrollSpeed, std::list<Bullet>& b) {
            sEnemy& e = *dynamic_cast<sEnemy*>(&s);
            constexpr float fDelay = 0.01f;
            constexpr int nBullets = 50;
            constexpr float fTetha = ((float)PI * 2.0f / nBullets);

            e.dataFire[0] += fElapsedTime;
            if (e.dataFire[0] >= fDelay) {
                e.dataFire[0] -= fDelay;
                e.dataFire[1] += 0.1f;

                Bullet b;
                b.pos = e.pos + (olc::vf2d(24, 24)); // Remove magic numbers!
                b.vel = { 180.0f * cosf(e.dataFire[1]), 180.0f * sinf(e.dataFire[1]) };
                listEnemyBullets.push_back(b);
            }
        };

        olc::Sprite* enemyShip01 = new olc::Sprite("assets/enemyShip01.png");
        olc::Sprite* powerSpr = new olc::Sprite("assets/powerupSheet.png");
        olc::Sprite* powerSprProj1 = new olc::Sprite("assets/powerupProjectile1.png");
        olc::Sprite* powerSprProj2 = new olc::Sprite("assets/powerupProjectile2.png");
        float coldTime = 120.0f;
        listSpawns = {
            new sEnemyDefiniton(coldTime + 60.00, enemyShip01, 0.5f, Move_SinusoidWide, Fire_Straigt2, 3.0f),
            new sPowerUpDefiniton(coldTime + 30.00, powerSprProj1, 0.5f, Move_Bounce, Fire_None, powerUpType::GREEN),
            new sPowerUpDefiniton(coldTime + 120.00, powerSpr, 0.5f, Move_Bounce, Fire_None, powerUpType::DEFAULT),
            new sPowerUpDefiniton(coldTime + 240.00, powerSprProj2, 0.5f, Move_Bounce, Fire_None, powerUpType::BLUE),
            new sEnemyDefiniton(coldTime + 240.0, enemyShip01,  0.25f, Move_SinusoidNarrow, Fire_Straigt2, 3.0f),
            new sEnemyDefiniton(coldTime + 240.0, enemyShip01, 0.75f, Move_SinusoidNarrow, Fire_Straigt2, 3.0f),
            new sEnemyDefiniton(coldTime + 360.0, enemyShip01, 0.2f, Move_None, Fire_Straigt2, 3.0f),
            new sEnemyDefiniton(coldTime + 360.0, enemyShip01, 0.5f, Move_None, Fire_CirclePulse2, 3.0f),
            new sEnemyDefiniton(coldTime + 360.0, enemyShip01, 0.8f, Move_None, Fire_Straigt2, 3.0f),
            new sEnemyDefiniton(coldTime + 500.0, enemyShip01, 0.5f, Move_Fast, Fire_DeathSpiral, 3.0f),
        };


        return true;
    }

    bool GameUpdate(float fElapsedTime) {

        Clear(olc::BLACK);
        dWorldPos += fWorldSpeed * fElapsedTime;
        bg.Update(fElapsedTime);
        player.Update(fElapsedTime);
        exp->Update(fElapsedTime, player);


        while (!listSpawns.empty() && dWorldPos >= listSpawns.front()->dTriggerTime) {
            Spawn* currentSpawn = listSpawns.front();
            if (SpawnType::ENEMY == currentSpawn->type) {
                sEnemy e;
                e.def = dynamic_cast<sEnemyDefiniton*>(currentSpawn);
                e.pos = {
                    e.def->fOffset * ((float)ScreenWidth()) - (((float)e.def->spr->width) / 2),
                    0.0f - ((float)e.def->spr->height)
                };
                listSpawns.pop_front();
                listEnemies.push_back(e);
            }
            if (SpawnType::POWERUP == currentSpawn->type) {
                PowerUp p;
                p.def = dynamic_cast<sPowerUpDefiniton*>(currentSpawn);
                p.fWidth = ((int)p.def->spr->width) / 3; // remove magice numbers
                p.fHeight = ((int)p.def->spr->height);
                p.pos = {
                    ((float)(rand() % ScreenWidth())),
                    ((float)(rand() % ScreenHeight()))
                };
                listSpawns.pop_front();
                listPowerUp.push_back(p);
            }
        }


        for (auto& f : listFragments) {
            f.pos += (f.vel + olc::vf2d(0.0f, fWorldSpeed)) * fElapsedTime;
        }

        for (auto& e : listEnemies) {
            e.Update(fElapsedTime, fWorldSpeed, listEnemyBullets);
        }

        for (auto& p : listPowerUp) {
            p.Update(fElapsedTime, fWorldSpeed);
        }

        for (auto& b : listEnemyBullets) {
            b.Update(fElapsedTime, fWorldSpeed, player);
        }

        detectPlayerBulletCollision(fElapsedTime, player.listPlayerBullets, listEnemies, &listFragments);
        detectPlayerPowerUpCollision(fElapsedTime, player, listPowerUp);

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
        for (auto& e : listEnemies) DrawSprite(e.pos, e.def->spr, 1, olc::Sprite::Flip::VERT);
        SetPixelMode(olc::Pixel::NORMAL);

        for (auto& p : listPowerUp) {
            if (p.bBlink) {
                //DrawSprite(p.pos, p.def->spr);
                p.Draw(*this);
            }
        }

        for (auto& b : listEnemyBullets) FillCircle(b.pos, 3, olc::RED);

        for (auto& e : listExplosions) e->Draw();

        for (auto& f : listFragments) {
            Draw(f.pos, olc::Pixel(255, 255, 0, f.alpha));
            f.alpha -= (fElapsedTime * 0.005f);
        }

        //HUD
        DrawString(4, 4, "HEALTH:");
        FillRect(60, 4, ((int)(player.getHealth() / 100.0f * 576.0f)), 8, olc::GREEN); // Remove magic numbers!

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

// 1. add boss
// 2. add score
// 3. add main menu