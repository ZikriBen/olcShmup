#ifndef SCREEN_H
#define SCREEN_H


#include "olcPGEX_MiniAudio.h"
#include "olcPixelGameEngine.h"

#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Explosion.h"
#include "PowerUp.h"
#include "RetroMenu.h"
#include "ScrollingStarsBG.h"
#include "ZoomingStarsBG.h"
#include "ZoomingStarsJavBG.h"
#include "PlayerMovement.h"
#include "Difficulty.h"
constexpr float PI = 3.14159f;


#pragma once
static int gloalScore = 0;
static GameDifficulty globalGameDiff = GameDifficulty::NORMAL;
static bool soundOn = true;
static bool globalAutoFire = false;

class Screen
{
public:
	virtual void Create() = 0;
	virtual bool Run(float fElapsedTime) = 0;
	virtual void Destroy() = 0;
	virtual void Reset() = 0;

	// Johnnyg63: Lets add some sounds !!!
public:

	olc::MiniAudio miniAudio;		// We use miniAudio to play sounds throught out our game
	int32_t nMenuMusic_ID = -1;     // Stores the ID of the menu musice so it can be played & looped easliy
	std::string souMenuMusic = "./assets/sounds/ShmupBGMusic.mp3";       // Holds the full path to ShmupBGMusic.mp3
	std::string souLaserA1 = "./assets/sounds/laser_a1.mp3";             // Holds the full path to laser_a1.mp3
	std::string souBigExplosion = "./assets/sounds/big_explosion.wav";    // Holds the full path to big_explosion.way
	std::string souBeep = "./assets/sounds/one_beep-99630.mp3";
};

class StartScreen : public Screen {
public:
	StartScreen(olc::PixelGameEngine& pge) : pge(pge) {};

	olc::PixelGameEngine& pge;
	int printIndex = 0;
	float printSpeed = 30.0f; // 10 characters per second
	float timeAccumulator = 0.0f;
	int offsetY = 20;
	int currentLine = 0;
	std::vector<std::string> lines;
	float fStartDelay = 0.5f;
	float fStartDelayTimer = 0.0;
	bool spacePressed = false;
	float fEndDelayTimer = 0.0;
	float fEndDelay = 2.0f;
	std::string textToPrint5 = "Press SPACEBAR to continue...";
	float fBlinkTimer = 0.0f;
	bool bBlink = true;
	float blinkInterval = 0.4f;
	std::unique_ptr<olc::Sprite> logo = nullptr;
	ZoomingStarsJavBG bg{ pge };

	void Create() {
		logo = std::make_unique <olc::Sprite>("assets/images/logoLongS.png");

		lines.push_back("Galactic Havoc : Deep Space Assault");
		lines.push_back("Made by Ben Zikri");
		lines.push_back("Licensed under OLC - 3.");
		lines.push_back("2018 - 2024 OneLoneCoder.com");
		lines.push_back("All rights reserved.");
		bg.populateStars();

	};

	bool Run(float fElapsedTime) {
		pge.Clear(olc::BLACK);
		bg.Update(fElapsedTime);

		fStartDelayTimer += fElapsedTime;
		if (fStartDelayTimer < fStartDelay) {
			return true;
		}

		bg.Draw();

		if (spacePressed) {
			fEndDelayTimer += fElapsedTime;
			if (fEndDelayTimer < fEndDelay)
				return true;
			else
				return false;
		}
		for (int i = 0; i < lines.size(); ++i)
			pge.DrawString((pge.ScreenWidth() / 2) - (pge.GetTextSize(lines[i]).x / 2), 100 + i * offsetY, lines[i], olc::WHITE);

		pge.DrawSprite({ (pge.ScreenWidth() / 2) - (logo->width / 2), 300 }, logo.get());

		InputHandling();

		fBlinkTimer += fElapsedTime;
		if (fBlinkTimer > blinkInterval) {
			fBlinkTimer -= blinkInterval;
			bBlink = !bBlink;
		}

		if (!bBlink)
			pge.DrawString((pge.ScreenWidth() / 2) - (pge.GetTextSize(textToPrint5).x / 2), pge.ScreenHeight() - 20, textToPrint5, olc::DARK_GREY);

		return true;
	};

	void Destroy() {
		lines.clear();
		logo.reset();
	};

	void Reset() {
		printIndex = 0;
		timeAccumulator = 0.0f;
		currentLine = 0;
		fStartDelayTimer = 0.0;
		spacePressed = false;
		fEndDelayTimer = 0.0;
		fBlinkTimer = 0.0f;
		bBlink = true;
	}

	bool InputHandling() {
		if (pge.GetKey(olc::SPACE).bPressed)
			spacePressed = true;
		return true;
	}
};

class MenuScreen : public Screen {
public:
	MenuScreen(olc::PixelGameEngine& pge) : pge(pge), sprBG(nullptr), pos(0, 0) {};

	olc::PixelGameEngine& pge;
	std::unique_ptr<olc::Sprite> sprBG;
	std::unique_ptr<olc::Sprite> sprGFX;
	olc::vf2d pos;
	std::vector<std::string> lines;
	int currentSelection = 0;
	std::string sLastAction;
	menuobject mo;
	menumanager mm;

	void Create() {
		sprBG = std::make_unique <olc::Sprite>("assets/images/MainScreen640x480noText.png");
		sprGFX = std::make_unique <olc::Sprite>("./assets/images/RetroMenu2.png");

		lines.push_back("Start Game");
		lines.push_back("Options");

		mo["main"].SetTable(1, 3);
		mo["main"]["Difficulty"].SetTable(1, 3);
		mo["main"]["Sound"].SetTable(1, 2);
		mo["main"]["Auto Fire"].SetTable(1, 2);
		mo["main"]["Back"].SetID(100);
		mo["main"]["Difficulty"]["Easy"].SetID(101);
		mo["main"]["Difficulty"]["Normal"].SetID(102);
		mo["main"]["Difficulty"]["Hard"].SetID(103);

		mo["main"]["Sound"]["On"].SetID(104);
		mo["main"]["Sound"]["Off"].SetID(105);

		mo["main"]["Auto Fire"]["On"].SetID(106);
		mo["main"]["Auto Fire"]["Off"].SetID(107);

		mo.Build();

		// Johnngy63: This code plays the menu music in a loop
		// You do not need the 'this->' referance but I wanted you to see where I was getting this madness from
		// Basicilly I am asking the parent class (Screen) to load & play music for this child class (MenuScreen) only
		// Debug it to understand better
		if (soundOn) {
			this->nMenuMusic_ID = this->miniAudio.LoadSound(this->souMenuMusic);
			this->miniAudio.SetVolume(nMenuMusic_ID, 0.7f);
			this->miniAudio.Play(this->nMenuMusic_ID, true);
		}
	};

	bool Run(float fElapsedTime) {
		pge.Clear(olc::BLACK);
		int mid = (pge.ScreenWidth() / 2);
		int offsetY = (pge.ScreenHeight() / 2 + 170);

		pge.DrawSprite(olc::vf2d(0, 0), sprBG.get());
		pge.SetPixelMode(olc::Pixel::NORMAL);

		olc::Pixel p = olc::WHITE;

		for (int i = 0; i < lines.size(); ++i) {
			p = i == currentSelection ? olc::YELLOW : olc::WHITE;
			pge.DrawString(mid - (pge.GetTextSize(lines[i]).x), offsetY + (i * 20), lines[i], p, 2);
		}

		pge.SetPixelMode(olc::Pixel::NORMAL);
		if (!InputHandling())
			return false;

		mm.Draw(pge, sprGFX.get(), { mid - (pge.GetTextSize(lines[0]).x), offsetY - 80 });

		return true;
	};

	void CommandSwitch(int cmdID) {
		if (cmdID == 101) {
			globalGameDiff = GameDifficulty::EASY;
		}
		else if (cmdID == 102) {
			globalGameDiff = GameDifficulty::NORMAL;
		}
		else if (cmdID == 103) {
			globalGameDiff = GameDifficulty::HARD;
		}
		else if (cmdID == 104) {
			float vol = 0.7f;
			soundOn = true;
			this->miniAudio.SetVolume(nMenuMusic_ID, vol);
		}
		else if (cmdID == 105) {
			float vol = 0.0f;
			soundOn = false;
			this->miniAudio.SetVolume(nMenuMusic_ID, vol);
		}
		else if (cmdID == 106) {
			globalAutoFire = true;
		}
		else if (cmdID == 107) {
			globalAutoFire = false;
		}
	}

	void Destroy() {};

	void Reset() {
		currentSelection = 0;
		sLastAction = "";
	};

	bool InputHandling() {
		menuobject* command = nullptr;
		if (!mm.isOpen()) {
			if (pge.GetKey(olc::UP).bPressed || pge.GetKey(olc::Key::W).bPressed)
				currentSelection = ((int)(currentSelection - 1 + lines.size()) % lines.size());

			if (pge.GetKey(olc::DOWN).bPressed || pge.GetKey(olc::Key::S).bPressed)
				currentSelection = (currentSelection + 1) % lines.size();

			if (pge.GetKey(olc::ENTER).bPressed || pge.GetKey(olc::Key::SPACE).bPressed) {

				// Johnnyg63: Plays the laser sound when you press Enter||Space
				if (soundOn)
					this->miniAudio.Play(this->souLaserA1);
				if (currentSelection == 0)
					return false;
				else if (currentSelection == 1)
					mm.Open(&mo["main"]);
				else if (currentSelection == 2)
					return true;
				else
					return true;
			}
		}
		else {
			if (pge.GetKey(olc::UP).bPressed || pge.GetKey(olc::Key::W).bPressed)
				mm.OnUp();

			if (pge.GetKey(olc::DOWN).bPressed || pge.GetKey(olc::Key::S).bPressed)
				mm.OnDown();

			if (pge.GetKey(olc::Key::A).bPressed)
				mm.OnLeft();

			if (pge.GetKey(olc::Key::D).bPressed)
				mm.OnRight();

			if (pge.GetKey(olc::ENTER).bPressed || pge.GetKey(olc::Key::SPACE).bPressed)
			{
				// Johnnyg63: Plays the laser sound when you press Enter||Space
				if (soundOn)
					this->miniAudio.Play(this->souLaserA1);
				command = mm.OnConfirm();
			}


			if (pge.GetKey(olc::ESCAPE).bPressed || pge.GetKey(olc::Key::Z).bPressed)
				mm.OnBack();

			if (command != nullptr)
			{
				sLastAction = "Selected: " + command->GetName() + " ID: " + std::to_string(command->GetID());
				CommandSwitch(command->GetID());
				mm.Close();
			}
		}

		if (pge.GetKey(olc::Key::M).bPressed)
			mm.Open(&mo["main"]);

		return true;
	}
};

class GameScreen : public Screen {
public:
	GameScreen(olc::PixelGameEngine& pge) : pge(pge) {};
	olc::PixelGameEngine& pge;
	float fWorldSpeed = 40.0f;
	double dWorldPos = 0;
	Player player{ pge , miniAudio };
	PlayerMovement playerMovement{ pge, player };

	ScrollingStarsBG bg{ pge, fWorldSpeed, 200 };
	float fDescTimer = 0.0;
	float fDescViewTime = 2.0f;
	float fEndDelayTimer1 = 0.0f;
	float fEndDelayTimer2 = 0.0f;
	float fEndDelay1 = 1.0f;
	float fEndDelay2 = 1.0f;
	bool bGameOn = true;
	std::vector<std::unique_ptr<olc::Sprite>> listSprites;
	std::list<Spawn*> listSpawns;
	std::list<sEnemy*> listEnemies;
	std::list<Bullet> listEnemyBullets;
	std::list<Bullet> listFragments;
	std::list<Explosion*> listExplosions;
	std::list<PowerUp> listPowerUp;
	std::vector<void*> listToRemove;
	std::unique_ptr<sBoss> pBoss;
	bool bPlayerExp = false;
	Difficulty gameScreenDifficulty;
	float healthDecrementInterval = 0.07f; // Decrement health every 0.1 seconds
	float healthDecrementTimer = 0.0f;

	void Create() {
		bg.populateStars();
		GameScreen::Reset();

		listSprites.emplace_back(std::make_unique<olc::Sprite>("assets/images/enemyShip01.png"));
		listSprites.emplace_back(std::make_unique<olc::Sprite>("assets/images/enemyShip02.png"));
		listSprites.emplace_back(std::make_unique<olc::Sprite>("assets/images/enemyShip03.png"));
		listSprites.emplace_back(std::make_unique<olc::Sprite>("assets/images/boss2.png"));
		listSprites.emplace_back(std::make_unique<olc::Sprite>("assets/images/powerupSheet.png"));
		listSprites.emplace_back(std::make_unique<olc::Sprite>("assets/images/powerupProjectile1.png"));
		listSprites.emplace_back(std::make_unique<olc::Sprite>("assets/images/poweupProjectile2.png"));
		listSprites.emplace_back(std::make_unique<olc::Sprite>("assets/images/powerupHealthSheet.png"));

		CreateSpawns();
	};

	bool Run(float fElapsedTime) {
		dWorldPos += fWorldSpeed * fElapsedTime;

		Spawnning();

		// Update Functions
		bg.Update(fElapsedTime);
		playerMovement.Update(fElapsedTime);
		player.Update(fElapsedTime);

		for (auto& f : listFragments) {
			f.pos += (f.vel + olc::vf2d(0.0f, fWorldSpeed)) * fElapsedTime;
		}

		for (auto& e : listEnemies) {
			e->Update(fElapsedTime, fWorldSpeed, listEnemyBullets);
		}

		for (auto& p : listPowerUp) {
			p.Update(fElapsedTime, fWorldSpeed);
		}

		for (auto& b : listEnemyBullets) {
			b.Update(fElapsedTime, fWorldSpeed, player);
		}

		for (auto& exp : listExplosions) {
			exp->Update(fElapsedTime, player);
		}

		detectPlayerBulletCollision(fElapsedTime, player.listPlayerBullets, listEnemies, &listFragments);
		detectPlayerPowerUpCollision(fElapsedTime, player, listPowerUp);

		// Draw Functions
		pge.Clear(olc::BLACK);
		bg.Draw();

		if (fDescTimer < fDescViewTime) {
			pge.DrawString((pge.ScreenWidth() / 2) - (pge.GetTextSize("Dimension 1").x), 100, "Dimension 1", olc::WHITE, 2);
			pge.DrawString((pge.ScreenWidth() / 2) - (pge.GetTextSize("First Wave").x / 2), 120, "First Wave", olc::WHITE);
			fDescTimer += fElapsedTime;
		}

		switch (player.lifeState)
		{
		case Player::ALIVE:
			player.Draw();
			break;
		case Player::DYING: {
			if (!bPlayerExp) {
				Explosion* exp = new Explosion(pge, player.pos, new olc::Sprite("assets/images/explosion-spritesheet2.png"));
				bPlayerExp = true;
				listExplosions.push_back(exp);
				// Johnngy63: Play explosion
				if (soundOn)
					this->miniAudio.Play(this->souBigExplosion);
			}
			break;
		}
		case Player::DEAD:
			bGameOn = false;
			break;
		}

		pge.SetPixelMode(olc::Pixel::MASK);
		for (auto& e : listEnemies) e->Draw(pge);
		pge.SetPixelMode(olc::Pixel::NORMAL);

		for (auto& p : listPowerUp) {
			if (p.bBlink) {
				p.Draw(pge);
			}
		}

		for (auto& b : listEnemyBullets) pge.FillCircle(b.pos, 3, olc::RED);

		for (auto& e : listExplosions) e->Draw();

		for (auto& f : listFragments) {
			pge.Draw(f.pos, olc::Pixel(255, 255, 0, static_cast<uint8_t>(f.alpha)));
			f.alpha -= fElapsedTime * 0.005f;
		}

		//HUD
		pge.DrawString(4, 4, "HEALTH:");
		pge.FillRect(60, 4, static_cast<int>((player.getCurrentHealth() / static_cast<float>(player.getMaxHealth()) * 576.0)), 8, olc::GREEN); // Scale player's health bar
		pge.DrawString(4, 16, "SCORE: " + std::to_string(gloalScore));

		if (pBoss) {
			pge.DrawString(4, 450, "BOSS:");
			pge.FillRect(60, 450, static_cast<int>((pBoss.get()->def->fHealth / pBoss.get()->maxHealth) * 576.0f), 8, olc::RED);
		}

		cleanUp();

		if (listSpawns.empty() && listEnemies.empty() && listExplosions.empty() && listEnemyBullets.empty()) {
			bGameOn = false;
		}

		if (!bGameOn) {
			fEndDelayTimer1 += fElapsedTime;

			if (fEndDelayTimer1 < fEndDelay1) {
				return true;
			}

			healthDecrementTimer += fElapsedTime;

			if (healthDecrementTimer >= healthDecrementInterval) {
				healthDecrementTimer -= healthDecrementInterval;

				if (player.getCurrentHealth() > 1) {
					player.setCurrentHealth(player.getCurrentHealth() - 1);
					gloalScore += 100;
					if (soundOn)
						this->miniAudio.Play(this->souBeep);
					fEndDelayTimer2 = 0.0f;
				}
			}

			fEndDelayTimer2 += fElapsedTime;

			if (fEndDelayTimer2 < fEndDelay2) {
				return true;
			}
			else return false;
		}

		return true;
	};

	void Destroy() {

		for (auto& spawn : listSpawns)
			delete spawn;
		listSpawns.clear();

		for (Explosion* exp : listExplosions)
			delete exp;
		listExplosions.clear();

		for (sEnemy* enemy : listEnemies)
			delete enemy;
		listEnemies.clear();

		listPowerUp.clear();

		for (auto& b : listEnemyBullets) {
			b.remove = true;
		}
		listEnemyBullets.clear();

		for (auto& f : listFragments) {
			f.remove = true;
		}
		listFragments.clear();

		for (auto& b : player.listPlayerBullets) {
			b.remove = true;
		}
		player.listPlayerBullets.clear();

		for (auto* p : listToRemove)
			delete p;
		listToRemove.clear();

		if (pBoss) {
			pBoss.release();
		}
	};

	void Reset() {
		bGameOn = true;
		pBoss = nullptr;
		gameScreenDifficulty.setDifficulty(globalGameDiff);
		player.reset();
		player.pos = olc::vf2d(((float)pge.ScreenWidth() / 2.0f) - player.getWidth() / 2.0f, ((float)pge.ScreenHeight() - player.getHeight() - 50.0f));
		player.lifeState = Player::ALIVE;
		player.setMaxHealth(gameScreenDifficulty.diffMap["PlayerHealth"]);
		player.setCurrentHealth(gameScreenDifficulty.diffMap["PlayerHealth"]);
		player.setSoundOn(soundOn);
		fWorldSpeed = gameScreenDifficulty.diffMap["WorldSpeed"];
		bPlayerExp = false;
		player.bAutoFire = globalAutoFire;
		fDescTimer = 0.0f;
		dWorldPos = 0;
		gloalScore = 0;
		fEndDelayTimer1 = 0.0f;
		fEndDelayTimer2 = 0.0f;
	};

	void CreateSpawns() {
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
			bGameOn = false;
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
				listEnemyBullets.push_back(b);
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
				b.pos = e.pos + (olc::vf2d((e.def->iWidth / 2.0f), e.def->iHeight));
				b.vel = { 180.0f * cosf(e.dataFire[1]), 180.0f * sinf(e.dataFire[1]) };
				listEnemyBullets.push_back(b);
			}
		};

		float coldTime = 120.0f;
		float enemyHealth = gameScreenDifficulty.diffMap["EnemyHealth"];
		float bossHealth = gameScreenDifficulty.diffMap["BossHealth"];

		listSpawns = {
			new sEnemyDefinition(coldTime + 60.0f,     listSprites[1].get(), 0.5f, Move_None,              Fire_Straigt2,     enemyHealth, (listSprites[1].get()->width),     listSprites[1].get()->height, 300),
			new sPowerUpDefinition(coldTime + 120.0f,  listSprites[5].get(), 0.5f, Move_Bounce,            Fire_None,         PowerUpType::GREEN, 100),
			new sEnemyDefinition(coldTime + 180.0f,    listSprites[1].get(), 0.2f, Move_None,              Fire_CirclePulse2, enemyHealth, (listSprites[1].get()->width),     listSprites[1].get()->height, 300),
			new sEnemyDefinition(coldTime + 180.0f,    listSprites[1].get(), 0.8f, Move_None,              Fire_CirclePulse2, enemyHealth, (listSprites[1].get()->width),     listSprites[1].get()->height, 300),
			new sEnemyDefinition(coldTime + 240.0f,    listSprites[1].get(), 0.5f, Move_SinusoidNarrow,    Fire_Straigt2,     enemyHealth, (listSprites[1].get()->width),     listSprites[1].get()->height, 300),
			new sPowerUpDefinition(coldTime + 200.00f, listSprites[4].get(), 0.5f, Move_Bounce,            Fire_None,         PowerUpType::DEFAULT, 100),
			new sEnemyDefinition(coldTime + 260.0f,    listSprites[0].get(), 0.3f, Move_SinusoidWide,      Fire_Straigt2,     enemyHealth, (listSprites[0].get()->width),     listSprites[0].get()->height, 300),
			new sEnemyDefinition(coldTime + 360.0f,    listSprites[0].get(), 0.3f, Move_Fast,              Fire_Straigt2,     enemyHealth, (listSprites[0].get()->width),     listSprites[0].get()->height, 300),
			new sEnemyDefinition(coldTime + 420.0f,    listSprites[0].get(), 0.1f, Move_Fast,              Fire_Straigt2,     enemyHealth, (listSprites[0].get()->width),     listSprites[0].get()->height, 300),
			new sEnemyDefinition(coldTime + 420.0f,    listSprites[0].get(), 0.9f, Move_Fast,              Fire_Straigt2,     enemyHealth, (listSprites[0].get()->width),     listSprites[0].get()->height, 300),
			new sEnemyDefinition(coldTime + 500.0f,    listSprites[0].get(), 0.5f, Move_None,              Fire_DeathSpiral,  enemyHealth, (listSprites[0].get()->width),     listSprites[0].get()->height, 300),
			new sEnemyDefinition(coldTime + 560.0f,    listSprites[0].get(), 0.5f, Move_None,              Fire_CirclePulse2, enemyHealth, (listSprites[0].get()->width),     listSprites[0].get()->height, 300),
			new sEnemyDefinition(coldTime + 600.0f,    listSprites[0].get(), 0.6f, Move_Slow,              Fire_Straigt2,     enemyHealth, (listSprites[0].get()->width),     listSprites[0].get()->height, 300),
			new sEnemyDefinition(coldTime + 720.0f,    listSprites[0].get(), 0.4f, Move_Fast,              Fire_Straigt2,     enemyHealth, (listSprites[0].get()->width),     listSprites[0].get()->height, 300),
			new sEnemyDefinition(coldTime + 780.0f,    listSprites[0].get(), 0.7f, Move_SinusoidWide,      Fire_Straigt2,     enemyHealth, (listSprites[0].get()->width),     listSprites[0].get()->height, 300),
			new sPowerUpDefinition(coldTime + 800.00f, listSprites[6].get(), 0.9f, Move_Bounce,            Fire_None,         PowerUpType::BLUE, 100),
			new sEnemyDefinition(coldTime + 840.0f,    listSprites[2].get(), 0.3f, Move_None,              Fire_Straigt2,     enemyHealth, (listSprites[2].get()->width),     listSprites[2].get()->height, 300),
			new sEnemyDefinition(coldTime + 900.0f,    listSprites[2].get(), 0.8f, Move_None,              Fire_CirclePulse2, enemyHealth, (listSprites[2].get()->width),     listSprites[2].get()->height, 300),
			new sPowerUpDefinition(coldTime + 930.00f, listSprites[7].get(), 0.5f, Move_Bounce,            Fire_None,         PowerUpType::HEALTH, 100),
			new sEnemyDefinition(coldTime + 960.0f,    listSprites[2].get(), 0.2f, Move_None,              Fire_CirclePulse2, enemyHealth, (listSprites[2].get()->width),     listSprites[2].get()->height, 300),
			new sEnemyDefinition(coldTime + 1020.0f,   listSprites[2].get(), 0.5f, Move_SinusoidNarrow,    Fire_Straigt2,     enemyHealth, (listSprites[2].get()->width),     listSprites[2].get()->height, 300),
			new sEnemyDefinition(coldTime + 1080.0f,   listSprites[2].get(), 0.9f, Move_Slow,              Fire_Straigt2,     enemyHealth, (listSprites[2].get()->width),     listSprites[2].get()->height, 300),
			new sBossDefiniton(coldTime + 1500.0f,     listSprites[3].get(), 0.5f, Move_SinusoidWideInf,   Fire_CirclePulse2, 20.0f,       (listSprites[3].get()->width / 5), listSprites[3].get()->height, 1000),
			//new sEnemyDefinition(coldTime + 300, listSprites[0], 0.5f, Move_None, Fire_End, 3.0f, (listSprites[0]->width), listSprites[0]->height),
		};

	}

	void Spawnning() {
		// Spawns
		while (!listSpawns.empty() && dWorldPos >= listSpawns.front()->dTriggerTime) {
			Spawn* currentSpawn = listSpawns.front();
			if (SpawnType::ENEMY == currentSpawn->type) {
				sEnemy* pEenemy = new sEnemy(dynamic_cast<sEnemyDefinition*>(currentSpawn));
				pEenemy->pos = {
					pEenemy->def->fOffset * ((float)pge.ScreenWidth()) - (((float)pEenemy->def->iWidth) / 2),
					0.0f - ((float)pEenemy->def->iHeight)
				};
				listSpawns.pop_front();
				listEnemies.push_back(pEenemy);
			}
			else if (SpawnType::POWERUP == currentSpawn->type) {
				PowerUp p;
				p.def = dynamic_cast<sPowerUpDefinition*>(currentSpawn);
				p.fWidth = ((int)p.def->spr->width) / 3; // remove magic numbers
				p.fHeight = ((int)p.def->spr->height);
				p.pos = {
					((float)(rand() % pge.ScreenWidth())),
					((float)(rand() % pge.ScreenHeight()))
				};
				listSpawns.pop_front();
				listPowerUp.push_back(p);
			}
			else if (SpawnType::BOSS == currentSpawn->type) {
				pBoss = std::make_unique<sBoss>(dynamic_cast<sBossDefiniton*>(currentSpawn));
				pBoss.get()->maxHealth = pBoss.get()->def->fHealth;
				pBoss.get()->fWidth = ((int)pBoss.get()->def->iWidth);
				pBoss.get()->fHeight = ((int)pBoss.get()->def->iHeight);
				pBoss.get()->pos = {
					pBoss.get()->def->fOffset * ((float)pge.ScreenWidth()) - (((float)pBoss.get()->def->iWidth) / 2),
					0.0f - ((float)pBoss->def->iHeight)
				};
				listSpawns.pop_front();
				listEnemies.push_back(pBoss.get());
			}
		}
	}

	void detectPlayerBulletCollision(float fElapsedTime, std::list<Bullet>& playerBullets, std::list<sEnemy*>& listEnemies, std::list<Bullet>* listFragments) {
		for (auto& b : playerBullets) {
			b.pos += (b.vel + olc::vf2d(0.0f, fWorldSpeed)) * fElapsedTime;
			for (auto& e : listEnemies)
				if ((b.pos - (e->pos + olc::vf2d(((float)e->def->iWidth / 2.0f), ((float)e->def->iHeight / 2.0f)))).mag2() < ((float)(e->def->iWidth * e->def->iHeight / 2.0f))) { // Remove magic numbers!
					b.remove = true;
					e->def->fHealth -= 1.0f;
					if (e->def->fHealth <= 0) {
						gloalScore += gameScreenDifficulty.diffMap["ScoreFactor"] * e->def->score;
						// Johnngy63: Play explosion
						if (soundOn)
							this->miniAudio.Play(this->souBigExplosion);

						for (int i = 0; i < 500; ++i) {
							float fAngle = ((float)rand() / (float)RAND_MAX * 2.0f * PI);
							float fSpeed = ((float)rand() / (float)RAND_MAX * 200.0f + 50.0f);
							listFragments->push_back({ e->pos + olc::vf2d(e->def->iWidth / 2.0f , e->def->iHeight / 2.0f), {fSpeed * cosf(fAngle), fSpeed * sinf(fAngle)}, false, 255 });
						}
					}
				}
		}
	}

	void detectPlayerPowerUpCollision(float fElapsedTime, Player& player, std::list<PowerUp>& listPowerUp) {
		for (auto& p : listPowerUp) {
			if ((p.pos - (player.pos + olc::vf2d(((float)player.getWidth() / 2.0f), ((float)player.getWidth() / 2.0f)))).mag2() < powf(((float)player.getWidth() / 2.0f), 2.0f)) {
				if (p.def->type == PowerUpType::DEFAULT) {
					player.setPowerUpLeve(1);
				}
				else if (p.def->type == PowerUpType::GREEN) {
					if (player.ProjectileType == static_cast<int>(PowerUpType::GREEN)) {
						player.setPowerUpLeve(1);
					}
					else {
						player.ProjectileType = Bullet::GREEN;
					}
				}
				else if (p.def->type == PowerUpType::BLUE) {
					if (player.ProjectileType == static_cast<int>(PowerUpType::BLUE)) {
						player.setPowerUpLeve(1);
					}
					else {
						player.ProjectileType = Bullet::BLUE;
					}
				}
				else if (p.def->type == PowerUpType::HEALTH) {
					player.setCurrentHealth(std::min(player.getCurrentHealth() + (player.getMaxHealth() / 3), player.getMaxHealth()));
				}
				gloalScore += p.def->score;
				p.remove = true;
			}
		}
	}

	void cleanUp() {

		for (auto it = listEnemies.begin(); it != listEnemies.end();) {
			if ((*it)->pos.y >= static_cast<float>(pge.ScreenHeight()) || (*it)->def->fHealth <= 0) {
				listToRemove.push_back(static_cast<void*>(*it));
				it = listEnemies.erase(it);
			}
			else {
				++it;
			}
		}

		for (auto it = listExplosions.begin(); it != listExplosions.end();) {
			if ((*it)->remove) {
				listToRemove.push_back(static_cast<void*>(*it));
				it = listExplosions.erase(it);
			}
			else {
				++it;
			}
		}

		listEnemyBullets.remove_if([&](const Bullet& b) {return b.pos.x < 0 || b.pos.x>pge.ScreenWidth() || b.pos.y <0 || b.pos.y > pge.ScreenHeight() || b.remove; });

		listFragments.remove_if([&](const Bullet& b) {return b.pos.x<0 || b.pos.x>pge.ScreenWidth() || b.pos.y <0 || b.pos.y > pge.ScreenHeight() || b.remove; });

		listPowerUp.remove_if([&](const PowerUp& p) {return p.remove; });
	}
};

class IntroScreen : public Screen {
public:
	IntroScreen(olc::PixelGameEngine& pge) : pge(pge) {};

	olc::PixelGameEngine& pge;
	int printIndex = 0;
	float printSpeed = 30.0f; // 10 characters per second
	float timeAccumulator = 0.0f;
	int offsetY = 30;
	int currentLine = 0;
	std::vector<std::string> lines;
	float fStartDelay = 1.0f;
	float fStartDelayTimer = 0.0;
	bool spacePressed = false;
	float fEndDelayTimer = 0.0;
	float fEndDelay = 2.0f;
	std::string textToPrint5 = "Press SPACEBAR to continue...";
	float fBlinkTimer = 0.0f;
	bool bBlink = true;
	float blinkInterval = 0.4f;

	void Create() {
		lines.push_back("Welcome to Galactic Havoc: Deep Space Assault!");
		lines.push_back("Take command of the last bastion against cosmic chaos.");
		lines.push_back("Navigate through perilous skies and vanquish hordes of alien foes.");
		lines.push_back("Upgrade your ship with powerful weapons and defenses.");
		lines.push_back("Unleash devastating special attacks to turn the tide of battle.");
		lines.push_back("Explore mysterious galaxies and uncover ancient secrets.");
		lines.push_back("The fate of the universe rests in your hands!");
	};

	bool Run(float fElapsedTime) {
		pge.Clear(olc::BLACK);

		fStartDelayTimer += fElapsedTime;
		if (fStartDelayTimer < fStartDelay) {
			return true;
		}

		typeWriter(fElapsedTime, offsetY);

		if (spacePressed) {
			fEndDelayTimer += fElapsedTime;
			if (fEndDelayTimer < fEndDelay)
				return true;
			else
				return false;
		}

		InputHandling();

		fBlinkTimer += fElapsedTime;
		if (fBlinkTimer > blinkInterval) {
			fBlinkTimer -= blinkInterval;
			bBlink = !bBlink;
		}

		if (!bBlink)
			pge.DrawString((pge.ScreenWidth() / 2) - (pge.GetTextSize(textToPrint5).x / 2), pge.ScreenHeight() - 20, textToPrint5, olc::DARK_GREY);

		return true;
	};

	void Destroy() {};

	void Reset() {
		printIndex = 0;
		timeAccumulator = 0.0f;
		currentLine = 0;
		fStartDelayTimer = 0.0;
		spacePressed = false;
		fEndDelayTimer = 0.0;
		fBlinkTimer = 0.0f;
		bBlink = true;
	};

	bool typeWriter(float fElapsedTime, int offsetY) {
		timeAccumulator += fElapsedTime;

		// Print already typed lines
		for (int i = 0; i < currentLine; ++i) {
			pge.DrawString((pge.ScreenWidth() / 2) - (pge.GetTextSize(lines[i]).x / 2), 10 + i * offsetY, lines[i], olc::WHITE);
		}

		// All lines are printed, do something else or stop typing
		if (currentLine >= lines.size()) {
			return false;
		}

		// Print the current line with typed characters
		if (currentLine < lines.size()) {
			std::string currentText = lines[currentLine].substr(0, printIndex);
			pge.DrawString((pge.ScreenWidth() / 2) - (pge.GetTextSize(lines[currentLine]).x / 2), 10 + currentLine * offsetY, currentText, olc::WHITE);
		}

		// Check if it's time to print another character
		if (timeAccumulator >= 1.0f / printSpeed && printIndex < lines[currentLine].size()) {
			printIndex++;
			timeAccumulator = 0.0f;
		}
		else if (printIndex >= lines[currentLine].size()) {
			currentLine++;
			printIndex = 0;
		}

		return true;
	}

	bool InputHandling() {
		if (pge.GetKey(olc::SPACE).bPressed) {
			spacePressed = true;
		}
		return true;
	}
};

class GameOverScreen : public Screen {
public:
	GameOverScreen(olc::PixelGameEngine& pge) : pge(pge) {};

	olc::PixelGameEngine& pge;
	int offsetY = 50;
	std::string textToPrint1 = "Game Over!";
	std::string textToPrint2 = "Well Done, your score: ";
	std::string textToPrint3 = "See you space cowboy...";
	std::string textToPrint4 = "Press SPACEBAR to continue";
	float fBlinkTimer = 0.0f;
	bool bBlink = true;
	float blinkInterval = 0.4f;
	float fStartDelay = 2.0f;
	bool spacePressed = false;
	float fStartDelayTimer = 0.0;

	void Create() {
		spacePressed = false;
		fBlinkTimer = 0.0f;
		bBlink = true;
	};

	bool Run(float fElapsedTime) {
		pge.Clear(olc::BLACK);
		pge.DrawString((pge.ScreenWidth() / 2) - (pge.GetTextSize(textToPrint1).x), 100, textToPrint1, olc::WHITE, 2);
		pge.DrawString((pge.ScreenWidth() / 2) - (pge.GetTextSize(textToPrint2).x / 2), 220, textToPrint2, olc::WHITE);
		pge.DrawString((pge.ScreenWidth() / 2) - (pge.GetTextSize(std::to_string(gloalScore)).x), 250, std::to_string(gloalScore), olc::WHITE, 2);
		pge.DrawString((pge.ScreenWidth() / 2) - (pge.GetTextSize(textToPrint3).x / 2), pge.ScreenHeight() - 60, textToPrint3, olc::WHITE);


		fStartDelayTimer += fElapsedTime;
		if (fStartDelayTimer < fStartDelay) {
			return true;
		}

		InputHandling();

		if (spacePressed) {
			return false;
		}


		fBlinkTimer += fElapsedTime;
		if (fBlinkTimer > blinkInterval) {
			fBlinkTimer -= blinkInterval;
			bBlink = !bBlink;
		}

		if (!bBlink)
			pge.DrawString((pge.ScreenWidth() / 2) - (pge.GetTextSize(textToPrint4).x / 2), pge.ScreenHeight() - 20, textToPrint4, olc::DARK_GREY);

		return true;

	};

	void Destroy() {};

	void Reset() {
		bBlink = true;
		fStartDelay = 2.0f;
		spacePressed = false;
		fStartDelayTimer = 0.0;
	};

	bool InputHandling() {
		if (pge.GetKey(olc::SPACE).bPressed)
			spacePressed = true;
		return true;
	}
};

#endif //SCREEN_H
