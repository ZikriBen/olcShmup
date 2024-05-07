#include "Player.h"

Player::Player(olc::PixelGameEngine& pge) : pge(pge) {
	sprPlayer = new olc::Sprite("assets/playerShip.png");
	pos = { (float)pge.ScreenWidth() / 2, (float)pge.ScreenHeight() / 2 };
	speed = 200.f;
	health = 100.0f;
	fGunReloadTimer = 0.0f;
	fGunReloadDelay = 0.2f;
	bCanFire = true;
}


void Player::UpdatePosition(float fElapsedTime) {
	pos.y += (40.0f * fElapsedTime) * 0.5;

	if (pge.GetKey(olc::W).bHeld) pos.y -= speed * fElapsedTime;
	if (pge.GetKey(olc::S).bHeld) pos.y += speed * fElapsedTime;
	if (pge.GetKey(olc::A).bHeld) pos.x -= speed * fElapsedTime;
	if (pge.GetKey(olc::D).bHeld) pos.x += speed * fElapsedTime;

	if (pos.x <= 0) pos.x = 0;
	if (pos.y <= 0) pos.x = 0;

	if (pos.x + 48.0f >= (float)pge.ScreenWidth()) pos.x = (float)pge.ScreenWidth() - 48.0f;
	if (pos.y + 48.0f >= (float)pge.ScreenHeight()) pos.y = (float)pge.ScreenHeight() - 48.0f;

	fGunReloadTimer += fElapsedTime;

	if (fGunReloadTimer >= fGunReloadDelay) {
		bCanFire = true;
		fGunReloadTimer -= fGunReloadDelay;
	}

	if (pge.GetKey(olc::SPACE).bHeld || pge.GetMouse(0).bHeld) {
		if (bCanFire) {
			sBullet b;
			b.pos = { pos.x + 24.0f, pos.y };
			b.vel = { 0.0f, -200.0f };
			listPlayerBullets.push_back(b);
			bCanFire = false;
		}
	}

	listPlayerBullets.remove_if([&](const sBullet& b) {return b.pos.x<0 || b.pos.x>pge.ScreenWidth() || b.pos.y <0 || b.pos.y>pge.ScreenHeight() || b.remove; });
}

void Player::Draw() {
	pge.DrawSprite(pos, sprPlayer);
}