#ifndef SCREEN_H
#define SCREEN_H
#include "olcPixelGameEngine.h"
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Background.h"
#include "Explosion.h"
#include "PowerUp.h"

constexpr float PI = 3.14159f;
#pragma once
class Screen
{
public:
	virtual void Create() = 0;
	virtual bool Run(float fElapsedTime) = 0;
	virtual void Destroy() = 0;
};

class MenuScreen : public Screen {
public:
	MenuScreen(olc::PixelGameEngine& pge) : pge(pge), sprBG(nullptr), pos(0,0){};

	void Create() {
		sprBG = new olc::Sprite("assets/MainScreen640x480noText.png");

		lines.push_back("Start Game");
		lines.push_back("Options");
		lines.push_back("About");
		lines.push_back("Exit");


		olc::vf2d pos = olc::vf2d(0, 0);

	};
	
	bool Run(float fElapsedTime) {
		pge.Clear(olc::BLACK);
		int mid = (pge.ScreenWidth() / 2);
		int offsetY = (pge.ScreenHeight() / 2 + 160);

		pge.DrawSprite(pos, sprBG);

		pge.SetPixelMode(olc::Pixel::NORMAL);

		olc::Pixel p = olc::WHITE;
		
		
		for (int i = 0; i < lines.size(); ++i) {
			p = i == currentSelection ? olc::YELLOW : olc::WHITE;
			pge.DrawString(mid - (pge.GetTextSize(lines[i]).x), offsetY + (i * 20), lines[i], p, 2);
		}

		pge.SetPixelMode(olc::Pixel::NORMAL);
		if (!InputHandling()) {
			return false;
		}

		return true;
	};
	
	void Destroy() {
		delete sprBG;
		lines.clear();
	};

	bool InputHandling() {
		if (pge.GetKey(olc::W).bPressed || pge.GetKey(olc::UP).bPressed) currentSelection = (currentSelection - 1 + lines.size()) % lines.size();
		if (pge.GetKey(olc::S).bPressed || pge.GetKey(olc::DOWN).bPressed) currentSelection = (currentSelection + 1) % lines.size();
		if (pge.GetKey(olc::SPACE).bPressed || pge.GetKey(olc::ENTER).bPressed) {
			if (currentSelection == 0) {
				std::cout << "Start Game" << std::endl;
			}
			else if (currentSelection == lines.size() - 1) {
				return false;
			}
		}

		return true;
	}

	olc::PixelGameEngine& pge;
	olc::Sprite* sprBG;
	olc::vf2d pos;
	std::vector<std:: string> lines;
	int currentSelection = 0;
	bool gameStart = false;
};

class GameScreen : public Screen {
public:
	GameScreen(olc::PixelGameEngine& pge) : pge(pge){};

	void Create() {
		player.pos = olc::vf2d(((float)pge.ScreenWidth() / 2.0f) - player.getWidth() / 2.0f, ((float)pge.ScreenHeight() - player.getHeight() - 50.0f));
		player.lifeState = Player::ALIVE;
		bg.populateStars();
		exp = new Explosion(pge);
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

	};

	bool Run(float fElapsedTime) {
		pge.Clear(olc::BLACK);
		dWorldPos += fWorldSpeed * fElapsedTime;
		bg.Update(fElapsedTime);
		player.Update(fElapsedTime);
		exp->Update(fElapsedTime, player);

		if (fDescTimer < fDescViewTime) {
			pge.DrawString((pge.ScreenWidth() / 2) - (pge.GetTextSize("Dimension 1").x ), 100, "Dimension 1", olc::WHITE, 2);
			pge.DrawString((pge.ScreenWidth() / 2) - (pge.GetTextSize("First Wave").x / 2), 120, "First Wave", olc::WHITE);
			fDescTimer += fElapsedTime;
		}

		while (!listSpawns.empty() && dWorldPos >= listSpawns.front()->dTriggerTime) {
			Spawn* currentSpawn = listSpawns.front();
			if (SpawnType::ENEMY == currentSpawn->type) {
				sEnemy e;
				e.def = dynamic_cast<sEnemyDefiniton*>(currentSpawn);
				e.pos = {
					e.def->fOffset * ((float)pge.ScreenWidth()) - (((float)e.def->spr->width) / 2),
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
					((float)(rand() % pge.ScreenWidth())),
					((float)(rand() % pge.ScreenHeight()))
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

		pge.SetPixelMode(olc::Pixel::MASK);
		for (auto& e : listEnemies) pge.DrawSprite(e.pos, e.def->spr, 1, olc::Sprite::Flip::VERT);
		pge.SetPixelMode(olc::Pixel::NORMAL);

		for (auto& p : listPowerUp) {
			if (p.bBlink) {
				//DrawSprite(p.pos, p.def->spr);
				p.Draw(pge);
			}
		}

		for (auto& b : listEnemyBullets) pge.FillCircle(b.pos, 3, olc::RED);

		for (auto& e : listExplosions) e->Draw();

		for (auto& f : listFragments) {
			pge.Draw(f.pos, olc::Pixel(255, 255, 0, f.alpha));
			f.alpha -= (fElapsedTime * 0.005f);
		}

		//HUD
		pge.DrawString(4, 4, "HEALTH:");
		pge.FillRect(60, 4, ((int)(player.getHealth() / 100.0f * 576.0f)), 8, olc::GREEN); // Remove magic numbers!

		cleanUp();

		return true;
	};

	void Destroy() {
		
	};

	void detectPlayerBulletCollision(float fElapsedTime, std::list<Bullet>& playerBullets, std::list<sEnemy>& listEnemies, std::list<Bullet>* listFragments) {
		for (auto& b : playerBullets) {
			b.pos += (b.vel + olc::vf2d(0.0f, fWorldSpeed)) * fElapsedTime;
			for (auto& e : listEnemies)
				if ((b.pos - (e.pos + olc::vf2d(((float)e.def->spr->width / 2.0f), ((float)e.def->spr->width / 2.0f)))).mag2() < powf(((float)e.def->spr->width / 2.0f), 2.0f)) { // Remove magic numbers!
					b.remove = true;
					e.def->fHealth -= 1.0f;
					if (e.def->fHealth <= 0) {

						for (int i = 0; i < 500; ++i) {
							float fAngle = ((float)rand() / (float)RAND_MAX * 2.0f * PI);
							float fSpeed = ((float)rand() / (float)RAND_MAX * 200.0f + 50.0f);
							listFragments->push_back({ e.pos + olc::vf2d(24, 24), {fSpeed * cosf(fAngle), fSpeed * sinf(fAngle)}, false, 255 });
						}
					}
				}
		}
	}

	void detectPlayerPowerUpCollision(float fElapsedTime, Player& player, std::list<PowerUp>& listPowerUp) {
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
		listEnemies.remove_if([&](const sEnemy& e) {return e.pos.y >= ((float)pge.ScreenHeight()) || e.def->fHealth <= 0; });

		listEnemyBullets.remove_if([&](const Bullet& b) {return b.pos.x < 0 || b.pos.x>pge.ScreenWidth() || b.pos.y <0 || b.pos.y > pge.ScreenHeight() || b.remove; });

		listFragments.remove_if([&](const Bullet& b) {return b.pos.x<0 || b.pos.x>pge.ScreenWidth() || b.pos.y <0 || b.pos.y > pge.ScreenHeight() || b.remove; });

		listExplosions.remove_if([&](const Explosion* e) {return  e->remove; });

		listPowerUp.remove_if([&](const PowerUp& p) {return p.remove; });
	}


	olc::PixelGameEngine& pge;
	float fWorldSpeed = 40.0f;
	double dWorldPos = 0;

	Player player{ pge };
	Background bg{ pge, fWorldSpeed, 200 };
	Explosion* exp = nullptr;
	PowerUp* con;
	float fDescTimer = 0.0;
	float fDescViewTime = 2.0f;
	std::list<Spawn*> listSpawns;
	std::list<sEnemy> listEnemies;
	std::list<Bullet> listEnemyBullets;
	std::list<Bullet> listFragments;
	std::list<Explosion*> listExplosions;
	std::list<PowerUp> listPowerUp;
};

class IntroScreen : public Screen {
public:
	IntroScreen(olc::PixelGameEngine& pge) : pge(pge) {};
	
	olc::PixelGameEngine& pge;
	int printIndex = 0;
	float printSpeed = 20.0f; // 10 characters per second
	float timeAccumulator = 0.0f;
	int offsetY = 50;
	int currentLine = 0;
	std::vector<std::string> lines;
	float fStartDelay = 1.5f;
	float fStartDelayTimer = 0.0;
	bool spacePressed = false;
	float fEndDelayTimer = 0.0;
	float fEndDelay = 2.0f;
	std::string textToPrint5 = "Press SPACEBAR to continue...";
	float fBlinkTimer = 0.0f;
	bool bBlink = true;
	float blinkInterval = 0.4f;
	
	void Create() {
		
		std::string textToPrint1 = "Embark on Galactic Havoc: Deep Space Assault!";
		std::string textToPrint2 = "Command the last bastion against cosmic chaos.";
		std::string textToPrint3 = "Navigate perilous skies, vanquish alien foes.";
		std::string textToPrint4 = "Become the hero the universe awaits!";
		

		lines.push_back(textToPrint1);
		lines.push_back(textToPrint2);
		lines.push_back(textToPrint3);
		lines.push_back(textToPrint4);
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
			if (fEndDelayTimer < fEndDelay) {
				return true;
			}
			else
			{
				return false;
			}
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

	void Destroy() {
		
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


#endif //SCREEN_H
