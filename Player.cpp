#include "Player.h"
#include "CommandFactory.h"

Player::Player(olc::PixelGameEngine& pge) : pge(pge), lifeState(Player::ALIVE) {
	sprPlayerSheet = new olc::Sprite("assets/PlayerSpritesheet.png");
	pos = { (float)pge.ScreenWidth() / 2, (float)pge.ScreenHeight() / 2 };
	speed = 200.f;
	health = 100.0f;
	fGunReloadTimer = 0.0f;
	fGunReloadDelay = 0.1f;
	bCanFire = true;
	graphicState = STANDING;
	facingDirection = NORTH;
	fGraphicTimer = 0.0f;
	graphicCounter = 0;
	fWidth = 48.0f;
	fHeight = 58.0f;
	dead = false;
	factory = new ConcreteCommandFactory();
	ih  = new InputHandler(pge, *factory);
}


void Player::Update(float fElapsedTime) {
	if (health <= 0) {
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
	
	pos.y += (40.0f * fElapsedTime) * 0.5;

	if (pos.x <= 0) pos.x = 0;
	if (pos.y <= 0) pos.x = 0;

	if (pos.x + 48.0f >= (float)pge.ScreenWidth()) pos.x = (float)pge.ScreenWidth() - 48.0f;
	if (pos.y + 48.0f >= (float)pge.ScreenHeight()) pos.y = (float)pge.ScreenHeight() - 48.0f;

	fGunReloadTimer += fElapsedTime;

	if (fGunReloadTimer >= fGunReloadDelay) {
		bCanFire = true;
		fGunReloadTimer -= fGunReloadDelay;
	}

	ih->handleInput(*this, fElapsedTime);
	
	listPlayerBullets.remove_if([&](const Bullet& b) {return b.pos.x<0 || b.pos.x>pge.ScreenWidth() || b.pos.y <0 || b.pos.y>pge.ScreenHeight() || b.remove; });
}

void Player::Draw() {
	int nSheetOffsetX = 0;
	int nSheetOffsetY = 0;

	switch (graphicState)
	{
	case Player::STANDING:
		nSheetOffsetX = 0;
		nSheetOffsetY = 0;
		break;
	case Player::MOVING:
		nSheetOffsetX = graphicCounter * fWidth;
		nSheetOffsetY = facingDirection * fHeight;
		break;
	case Player::DEAD:
		break;
	default:
		break;
	}
	pge.SetPixelMode(olc::Pixel::MASK);
	pge.DrawPartialSprite(pos.x, pos.y, sprPlayerSheet, nSheetOffsetX, nSheetOffsetY, (int)fWidth, (int)fHeight, 1, 0);
	
	pge.SetPixelMode(olc::Pixel::NORMAL);
	
	for (auto& b : listPlayerBullets) 
		pge.FillCircle(b.pos, 3, olc::CYAN);
}

