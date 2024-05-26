#ifndef LEVEL_H
#define LEVEL_H
#pragma once
#include "Spawn.h"
#include "Screen.h"
#include "olcPixelGameEngine.h"

class Level {

public:
	GameScreen& screen;
	std::function<void(sSpawn&, float, float)> funcMove;
	std::function<void(sSpawn&, float, float, std::list<Bullet>&)> funcFire;
	
Level(GameScreen& screen) : screen(screen) {
		// Movement Patterns
		auto Move_None = [&](sSpawn& e, float fElapsedTime, float fScrollSpeed) {
			e.pos.y += fScrollSpeed * fElapsedTime;
		};

		auto Move_Slow = [&](sSpawn& e, float fElapsedTime, float fScrollSpeed) {
			e.pos.y += fScrollSpeed * fElapsedTime * 0.5f;

		};

		auto Move_Fast = [&](sSpawn& e, float fElapsedTime, float fScrollSpeed) {
			e.pos.y += fScrollSpeed * fElapsedTime * 2.0f;

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

		auto Move_SinusoidWideInf = [&](sSpawn& e, float fElapsedTime, float fScrollSpeed) {
			e.dataMove[0] += fElapsedTime;

			e.pos.x += 200.0f * cosf(e.dataMove[0]) * fElapsedTime;

			if (e.pos.y + (e.fHeight) >= 0.87f * pge.ScreenHeight())
				e.dataMove[1] = -1;
			if (e.pos.y <= 0)
				e.dataMove[1] = 1;

			e.pos.y += e.dataMove[1] * fScrollSpeed * fElapsedTime * 1.0f;

		};

		auto Move_Bounce = [&](sSpawn& e, float fElapsedTime, float fScrollSpeed) {
			// Update position based on velocity

			e.pos += e.velocity * fElapsedTime;

			// Check collision with screen boundaries
			if (e.pos.x < 0) {
				e.pos.x = 0;
				e.velocity.x *= -1;
			}
			else if (e.pos.x + e.fWidth > pge.ScreenWidth()) {
				e.pos.x = ((float)pge.ScreenWidth() - e.fWidth);
				e.velocity.x *= -1;
			}

			if (e.pos.y < 0) {
				e.pos.y = 0;
				e.velocity.y *= -1;
			}
			else if (e.pos.y + e.fHeight > pge.ScreenHeight()) {
				e.pos.y = ((float)pge.ScreenHeight() - e.fHeight);
				e.velocity.y *= -1;
			}
		};

		auto Fire_None = [&](sSpawn& e, float fElapsedTime, float fScrollSpeed, std::list<Bullet>& b) {};

		auto Fire_End = [&](sSpawn& e, float fElapsedTime, float fScrollSpeed, std::list<Bullet>& b) {
			screen.bGameOn = false;
		};

		auto Fire_Straigt2 = [&](sSpawn& s, float fElapsedTime, float fScrollSpeed, std::list<Bullet>& b) {
			sEnemy& e = *dynamic_cast<sEnemy*>(&s);
			constexpr float fDelay = 0.3f;
			e.dataFire[0] += fElapsedTime;
			if (e.dataFire[0] >= fDelay) {
				e.dataFire[0] -= fDelay;
				Bullet b;
				b.pos = e.pos + (olc::vf2d(((float)e.def->iWidth / 2.0f), ((float)e.def->iHeight)));
				b.vel = { 0.0f, 180.0f };
				screen.listEnemyBullets.push_back(b);
			}
		};

		auto Fire_CirclePulse2 = [&](sSpawn& s, float fElapsedTime, float fScrollSpeed, std::list<Bullet>& b) {
			sEnemy& e = *dynamic_cast<sEnemy*>(&s);
			constexpr float fDelay = 1.0f;
			constexpr int nBullets = 15;
			constexpr float fTetha = ((float)PI * 2.0f / nBullets);

			e.dataFire[0] += fElapsedTime;
			if (e.dataFire[0] >= fDelay) {
				e.dataFire[0] -= fDelay;
				for (int i = 0; i < nBullets; ++i) {
					Bullet b;
					b.pos = e.pos + (olc::vf2d(((float)e.def->iWidth / 2.0f), ((float)e.def->iHeight / 2.0f)));
					b.vel = { 180.0f * cosf(fTetha * i), 180.0f * sinf(fTetha * i) };
					screen.listEnemyBullets.push_back(b);
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
				b.pos = e.pos + (olc::vf2d((e.def->iWidth / 2.0f), e.def->iHeight));
				b.vel = { 180.0f * cosf(e.dataFire[1]), 180.0f * sinf(e.dataFire[1]) };
				screen.listEnemyBullets.push_back(b);
			}
		};
	}
	void createSpawns() {
		listSprites = {
			new olc::Sprite("assets/images/enemyShip01.png"),
			new olc::Sprite("assets/images/enemyShip02.png"),
			new olc::Sprite("assets/images/enemyShip03.png"),
			new olc::Sprite("assets/images/boss2.png"),
			new olc::Sprite("assets/images/powerupSheet.png"),
			new olc::Sprite("assets/images/powerupProjectile1.png"),
			new olc::Sprite("assets/images/powerupProjectile2.png"),
			new olc::Sprite("assets/images/powerupHealthSheet.png"),
		};

		float coldTime = 120.0f;
		float enemyHealth = screen.gameScreenDifficulty.diffMap["EnemyHealth"];
		float bossHealth = screen.gameScreenDifficulty.diffMap["BossHealth"];

		screen.listSpawns = {
			new sEnemyDefinition(coldTime + 60.00f,    screen.listSprites[1], 0.5f, Move_None,              Fire_Straigt2,     enemyHealth, (listSprites[1]->width),     listSprites[1]->height, 300),
			new sPowerUpDefinition(coldTime + 120.00f, screen.listSprites[5], 0.5f, Move_Bounce,            Fire_None,         PowerUpType::GREEN, 100),
			new sEnemyDefinition(coldTime + 180.0f,    screen.listSprites[1], 0.2f, Move_None,              Fire_CirclePulse2, enemyHealth, (listSprites[1]->width),     listSprites[1]->height, 300),
			new sEnemyDefinition(coldTime + 180.0f,    screen.listSprites[1], 0.8f, Move_None,              Fire_CirclePulse2, enemyHealth, (listSprites[1]->width),     listSprites[1]->height, 300),
			new sEnemyDefinition(coldTime + 240.0f,    screen.listSprites[1], 0.5f, Move_SinusoidNarrow,    Fire_Straigt2,     enemyHealth, (listSprites[1]->width),     listSprites[1]->height, 300),
			new sPowerUpDefinition(coldTime + 200.00f, screen.listSprites[4], 0.5f, Move_Bounce,            Fire_None,         PowerUpType::DEFAULT, 100),
			new sEnemyDefinition(coldTime + 260.0f,    screen.listSprites[0], 0.3f, Move_SinusoidWide,      Fire_Straigt2,     enemyHealth, (listSprites[0]->width),     listSprites[0]->height, 300),
			new sEnemyDefinition(coldTime + 360.0f,    screen.listSprites[0], 0.3f, Move_Fast,              Fire_Straigt2,     enemyHealth, (listSprites[0]->width),     listSprites[0]->height, 300),
			new sEnemyDefinition(coldTime + 420.0f,    screen.listSprites[0], 0.1f, Move_Fast,              Fire_Straigt2,     enemyHealth, (listSprites[0]->width),     listSprites[0]->height, 300),
			new sEnemyDefinition(coldTime + 420.0f,    screen.listSprites[0], 0.9f, Move_Fast,              Fire_Straigt2,     enemyHealth, (listSprites[0]->width),     listSprites[0]->height, 300),
			new sEnemyDefinition(coldTime + 500.0f,    screen.listSprites[0], 0.5f, Move_None,              Fire_DeathSpiral,  enemyHealth, (listSprites[0]->width),     listSprites[0]->height, 300),
			new sEnemyDefinition(coldTime + 560.0f,    screen.listSprites[0], 0.5f, Move_None,              Fire_CirclePulse2, enemyHealth, (listSprites[0]->width),     listSprites[0]->height, 300),
			new sEnemyDefinition(coldTime + 600.0f,    screen.listSprites[0], 0.6f, Move_Slow,              Fire_Straigt2,     enemyHealth, (listSprites[0]->width),     listSprites[0]->height, 300),
			new sEnemyDefinition(coldTime + 720.0f,    screen.listSprites[0], 0.4f, Move_Fast,              Fire_Straigt2,     enemyHealth, (listSprites[0]->width),     listSprites[0]->height, 300),
			new sEnemyDefinition(coldTime + 780.0f,    screen.listSprites[0], 0.7f, Move_SinusoidWide,      Fire_Straigt2,     enemyHealth, (listSprites[0]->width),     listSprites[0]->height, 300),
			new sPowerUpDefinition(coldTime + 800.00f, screen.listSprites[6], 0.9f, Move_Bounce,            Fire_None,         PowerUpType::BLUE, 100),
			new sEnemyDefinition(coldTime + 840.0f,    screen.listSprites[2], 0.3f, Move_None,              Fire_Straigt2,     enemyHealth, (listSprites[2]->width),     listSprites[2]->height, 300),
			new sEnemyDefinition(coldTime + 900.0f,    screen.listSprites[2], 0.8f, Move_None,              Fire_CirclePulse2, enemyHealth, (listSprites[2]->width),     listSprites[2]->height, 300),
			new sPowerUpDefinition(coldTime + 930.00f, screen.listSprites[7], 0.5f, Move_Bounce,            Fire_None,         PowerUpType::HEALTH, 100),
			new sEnemyDefinition(coldTime + 960.0f,    screen.listSprites[2], 0.2f, Move_None,              Fire_CirclePulse2, enemyHealth, (listSprites[2]->width),     listSprites[2]->height, 300),
			new sEnemyDefinition(coldTime + 1020.0f,   screen.listSprites[2], 0.5f, Move_SinusoidNarrow,    Fire_Straigt2,     enemyHealth, (listSprites[2]->width),     listSprites[2]->height, 300),
			new sEnemyDefinition(coldTime + 1080.0f,   screen.listSprites[2], 0.9f, Move_Slow,              Fire_Straigt2,     enemyHealth, (listSprites[2]->width),     listSprites[2]->height, 300),
			new sBossDefiniton(coldTime + 1500.0f,     screen.listSprites[3], 0.5f, Move_SinusoidWideInf,   Fire_CirclePulse2, 20.0f,       (listSprites[3]->width / 5), listSprites[3]->height, 1000),
			//new sEnemyDefinition(coldTime + 300, listSprites[0], 0.5f, Move_None, Fire_End, 3.0f, (listSprites[0]->width), listSprites[0]->height),
		};
	}
}
};
#endif // LEVEL_H