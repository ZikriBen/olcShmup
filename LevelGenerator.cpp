#include "LevelGenerator.h"
#include "Enemy.h"

void LevelGenerator::init(int32_t screenWidthNew, int32_t screenHeightNew, float enemyHealthNew, float bossHealthNew)
{
	screenWidth = screenWidthNew;
	screenHeight = screenHeightNew;
	enemyHealth = enemyHealthNew;
	bossHealth = bossHealthNew;

	Move_Bounce = [&](sSpawn& e, float fElapsedTime, float fScrollSpeed) {
		// Update position based on velocity

		e.pos += e.velocity * fElapsedTime;

		// Check collision with screen boundaries
		if (e.pos.x < 0) {
			e.pos.x = 0;
			e.velocity.x *= -1;
		}
		else if (e.pos.x + e.fWidth > screenWidth) {
			e.pos.x = ((float)screenWidth - e.fWidth);
			e.velocity.x *= -1;
		}

		if (e.pos.y < 0) {
			e.pos.y = 0;
			e.velocity.y *= -1;
		}
		else if (e.pos.y + e.fHeight > screenHeight) {
			e.pos.y = ((float)screenHeight - e.fHeight);
			e.velocity.y *= -1;
		}
	};

	Move_None = [&](sSpawn& e, float fElapsedTime, float fScrollSpeed) {
		e.pos.y += fScrollSpeed * fElapsedTime;
	};

	Move_Slow = [&](sSpawn& e, float fElapsedTime, float fScrollSpeed) {
		e.pos.y += fScrollSpeed * fElapsedTime * 0.5f;

	};

	Move_Fast = [&](sSpawn& e, float fElapsedTime, float fScrollSpeed) {
		e.pos.y += fScrollSpeed * fElapsedTime * 2.0f;

	};

	Move_SinusoidNarrow = [&](sSpawn& e, float fElapsedTime, float fScrollSpeed) {
		e.pos.y += fScrollSpeed * fElapsedTime * 1.0f;
		e.dataMove[0] += fElapsedTime;
		e.pos.x += 50.0f * cosf(e.dataMove[0]) * fElapsedTime;
	};

	Move_SinusoidWide = [&](sSpawn& e, float fElapsedTime, float fScrollSpeed) {
		e.pos.y += fScrollSpeed * fElapsedTime * 1.0f;
		e.dataMove[0] += fElapsedTime;
		e.pos.x += 200.0f * cosf(e.dataMove[0]) * fElapsedTime;
	};

	Move_SinusoidWideInf = [&](sSpawn& e, float fElapsedTime, float fScrollSpeed) {
		e.dataMove[0] += fElapsedTime;

		e.pos.x += 200.0f * cosf(e.dataMove[0]) * fElapsedTime;

		if (e.pos.y + (e.fHeight) >= 0.87f * screenHeight)
			e.dataMove[1] = -1;
		if (e.pos.y <= 0)
			e.dataMove[1] = 1;

		e.pos.y += e.dataMove[1] * fScrollSpeed * fElapsedTime * 1.0f;

	};

	Fire_None = [&](sSpawn& e, float fElapsedTime, float fScrollSpeed, std::list<Bullet>& listEnemyBullets) {};

	Fire_Straigt2 = [&](sSpawn& s, float fElapsedTime, float fScrollSpeed, std::list<Bullet>& listEnemyBullets) {
		sEnemy& e = *dynamic_cast<sEnemy*>(&s);
		constexpr float fDelay = 0.3f;
		e.dataFire[0] += fElapsedTime;
		if (e.dataFire[0] >= fDelay) {
			e.dataFire[0] -= fDelay;
			Bullet b;
			b.pos = e.pos + (olc::vf2d(((float)e.def->iWidth / 2.0f), ((float)e.def->iHeight)));
			b.vel = { 0.0f, 180.0f };
			listEnemyBullets.push_back(b);
		}
	};

	Fire_CirclePulse2 = [&](sSpawn& s, float fElapsedTime, float fScrollSpeed, std::list<Bullet>& listEnemyBullets) {
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
				listEnemyBullets.push_back(b);
			}
		}
	};

	Fire_DeathSpiral = [&](sSpawn& s, float fElapsedTime, float fScrollSpeed, std::list<Bullet>& listEnemyBullets) {
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
			listEnemyBullets.push_back(b);
		}
	};
}

void LevelGenerator::lvlOneGeneate(const std::vector<std::unique_ptr<olc::Sprite>> &listSprites, std::list<Spawn*>& listSpawns)
{
	float coldTime = 120.0f;

	listSpawns.push_back(new sEnemyDefinition(coldTime + 60.0f, listSprites[1].get(), 0.5f, Move_None, Fire_Straigt2, enemyHealth, (listSprites[1].get()->width), listSprites[1].get()->height, 300));
	listSpawns.push_back(new sPowerUpDefinition(coldTime + 120.0f, listSprites[5].get(), 0.5f, Move_Bounce, Fire_None, PowerUpType::GREEN, 100));
	listSpawns.push_back(new sEnemyDefinition(coldTime + 180.0f, listSprites[1].get(), 0.2f, Move_None, Fire_CirclePulse2, enemyHealth, (listSprites[1].get()->width), listSprites[1].get()->height, 300));
	listSpawns.push_back(new sEnemyDefinition(coldTime + 180.0f, listSprites[1].get(), 0.8f, Move_None, Fire_CirclePulse2, enemyHealth, (listSprites[1].get()->width), listSprites[1].get()->height, 300));
	listSpawns.push_back(new sEnemyDefinition(coldTime + 240.0f, listSprites[1].get(), 0.5f, Move_SinusoidNarrow, Fire_Straigt2, enemyHealth, (listSprites[1].get()->width), listSprites[1].get()->height, 300));
	listSpawns.push_back(new sPowerUpDefinition(coldTime + 200.00f, listSprites[4].get(), 0.5f, Move_Bounce, Fire_None, PowerUpType::DEFAULT, 100));
	listSpawns.push_back(new sEnemyDefinition(coldTime + 260.0f, listSprites[0].get(), 0.3f, Move_SinusoidWide, Fire_Straigt2, enemyHealth, (listSprites[0].get()->width), listSprites[0].get()->height, 300));
	listSpawns.push_back(new sEnemyDefinition(coldTime + 360.0f, listSprites[0].get(), 0.3f, Move_Fast, Fire_Straigt2, enemyHealth, (listSprites[0].get()->width), listSprites[0].get()->height, 300));
	listSpawns.push_back(new sEnemyDefinition(coldTime + 420.0f, listSprites[0].get(), 0.1f, Move_Fast, Fire_Straigt2, enemyHealth, (listSprites[0].get()->width), listSprites[0].get()->height, 300));
	listSpawns.push_back(new sEnemyDefinition(coldTime + 420.0f, listSprites[0].get(), 0.9f, Move_Fast, Fire_Straigt2, enemyHealth, (listSprites[0].get()->width), listSprites[0].get()->height, 300));
	listSpawns.push_back(new sEnemyDefinition(coldTime + 500.0f, listSprites[0].get(), 0.5f, Move_None, Fire_DeathSpiral, enemyHealth, (listSprites[0].get()->width), listSprites[0].get()->height, 300));
	listSpawns.push_back(new sEnemyDefinition(coldTime + 560.0f, listSprites[0].get(), 0.5f, Move_None, Fire_CirclePulse2, enemyHealth, (listSprites[0].get()->width), listSprites[0].get()->height, 300));
	listSpawns.push_back(new sEnemyDefinition(coldTime + 600.0f, listSprites[0].get(), 0.6f, Move_Slow, Fire_Straigt2, enemyHealth, (listSprites[0].get()->width), listSprites[0].get()->height, 300));
	listSpawns.push_back(new sEnemyDefinition(coldTime + 720.0f, listSprites[0].get(), 0.4f, Move_Fast, Fire_Straigt2, enemyHealth, (listSprites[0].get()->width), listSprites[0].get()->height, 300));
	listSpawns.push_back(new sEnemyDefinition(coldTime + 780.0f, listSprites[0].get(), 0.7f, Move_SinusoidWide, Fire_Straigt2, enemyHealth, (listSprites[0].get()->width), listSprites[0].get()->height, 300));
	listSpawns.push_back(new sPowerUpDefinition(coldTime + 800.00f, listSprites[6].get(), 0.9f, Move_Bounce, Fire_None, PowerUpType::BLUE, 100));
	listSpawns.push_back(new sEnemyDefinition(coldTime + 840.0f, listSprites[2].get(), 0.3f, Move_None, Fire_Straigt2, enemyHealth, (listSprites[2].get()->width), listSprites[2].get()->height, 300));
	listSpawns.push_back(new sEnemyDefinition(coldTime + 900.0f, listSprites[2].get(), 0.8f, Move_None, Fire_CirclePulse2, enemyHealth, (listSprites[2].get()->width), listSprites[2].get()->height, 300));
	listSpawns.push_back(new sPowerUpDefinition(coldTime + 930.00f, listSprites[7].get(), 0.5f, Move_Bounce, Fire_None, PowerUpType::HEALTH, 100));
	listSpawns.push_back(new sEnemyDefinition(coldTime + 960.0f, listSprites[2].get(), 0.2f, Move_None, Fire_CirclePulse2, enemyHealth, (listSprites[2].get()->width), listSprites[2].get()->height, 300));
	listSpawns.push_back(new sEnemyDefinition(coldTime + 1020.0f, listSprites[2].get(), 0.5f, Move_SinusoidNarrow, Fire_Straigt2, enemyHealth, (listSprites[2].get()->width), listSprites[2].get()->height, 300));
	listSpawns.push_back(new sEnemyDefinition(coldTime + 1080.0f, listSprites[2].get(), 0.9f, Move_Slow, Fire_Straigt2, enemyHealth, (listSprites[2].get()->width), listSprites[2].get()->height, 300));
	listSpawns.push_back(new sBossDefiniton(coldTime + 1500.0f, listSprites[3].get(), 0.5f, Move_SinusoidWideInf, Fire_CirclePulse2, 20.0f, (listSprites[3].get()->width / 5), listSprites[3].get()->height, 1000));
}
