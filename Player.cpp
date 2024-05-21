#include "Player.h"


Player::Player(olc::PixelGameEngine& pge, olc::MiniAudio& miniAudio) : pge(pge), miniAudio(miniAudio), lifeState(Player::ALIVE) {
	sprPlayerSheet = new olc::Sprite("assets/images/PlayerSpritesheet.png");
	pos = { (float)pge.ScreenWidth() / 2, (float)pge.ScreenHeight() / 2 };
	speed = 200.f;
	maxHealth = 20.0f;
	currentHealth = 20.0f;
	fGunReloadTimer = 0.0f;
	fGunReloadDelay = 0.3f;
	bCanFire = true;
	graphicState = STANDING;
	facingDirection = NORTH;
	fGraphicTimer = 0.0f;
	graphicCounter = 0;
	fWidth = 48.0f;
	fHeight = 58.0f;
	dead = false;


	listProjectileDef.push_back(ProjectileDef());
	listProjectileDef.push_back({ new olc::Sprite("assets/images/projectile_1-small.png"), 1 });
	listProjectileDef.push_back({ new olc::Sprite("assets/images/projectile_2.png"), 2 });
	
	for (int i = 0; i < listProjectileDef.size(); ++i) {
		listProjectileDef[i].offsetX = ((int)fWidth / 2);
		
		if (listProjectileDef[i].spr) {
			listProjectileDef[i].offsetX -= ((int)listProjectileDef[i].spr->width / 2);
			listProjectileDef[i].offsetY = ((int)listProjectileDef[i].spr->height / 2);
		}
	}
}

void Player::Update(float fElapsedTime) {
	if (currentHealth <= 0) {
		dead = true;
		lifeState = Player::DYING;
		return;
	}
	
	fGraphicTimer += fElapsedTime;
	if (fGraphicTimer > 0.2f) {
		fGraphicTimer -= 0.2f;
		graphicCounter++;
		graphicCounter %= 2;
	}
	
	pos.y += (40.0f * fElapsedTime) * 0.5f;

	if (pos.x <= 0) pos.x = 0;
	if (pos.y <= 0) pos.y = 0;
	if (pos.x + fWidth >= (float)pge.ScreenWidth()) pos.x = (float)pge.ScreenWidth() - fWidth;
	if (pos.y + fHeight >= (float)pge.ScreenHeight()) pos.y = (float)pge.ScreenHeight() - fHeight;

	fGunReloadTimer += fElapsedTime;

	if (fGunReloadTimer >= fGunReloadDelay) {
		bCanFire = true;
		fGunReloadTimer -= fGunReloadDelay;
	}

	//ih->handleInput(*this, fElapsedTime);
	listPlayerBullets.remove_if([&](const Bullet& b) {return b.pos.x<0 || b.pos.x>pge.ScreenWidth() || b.pos.y <0 || b.pos.y>pge.ScreenHeight() || b.remove; });
}

void Player::Draw() {
	int nSheetOffsetX = 0;
	int nSheetOffsetY = 0;

	switch (graphicState)
	{
	case Player::STANDING:
		nSheetOffsetX = ((int)fWidth) * graphicCounter;
		nSheetOffsetY = 0;
		break;
	case Player::MOVING:
		nSheetOffsetX = ((int)fWidth) * graphicCounter;
		nSheetOffsetY = ((int)fHeight) *facingDirection;
		break;
	case Player::DEAD:
		break;
	default:
		break;
	}
	pge.SetPixelMode(olc::Pixel::MASK);
	pge.DrawPartialSprite(((int)pos.x), ((int)pos.y), sprPlayerSheet, nSheetOffsetX, nSheetOffsetY, (int)fWidth, (int)fHeight, 1, 0);
	

	for (auto& b : listPlayerBullets)
		if (ProjectileType != 0) {
			pge.DrawSprite(b.pos, listProjectileDef[ProjectileType].spr);
		}
		else {
			pge.FillCircle(b.pos, 3, olc::CYAN);
		}
	
	pge.SetPixelMode(olc::Pixel::NORMAL);
}

void Player::reset()
{
	//graphicState = Player::ALIVE;
	fGunReloadTimer = 0.0f;
	bCanFire = true;
	fGraphicTimer = 0.0f;
	currentHealth = 100.0f;
	maxHealth = 100.0f;
	graphicState = STANDING;
	facingDirection = NORTH;
	graphicCounter = 0;
	dead = false;
	ProjectileType = 1;
	powerUpLevel = 1;
}
