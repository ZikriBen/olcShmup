#include "Player.h"

Player::Player(): olc::PixelGameEngine() {
	sprPlayer = new olc::Sprite("assets/playerShip.png");
	pos = { (float)ScreenWidth() / 2, (float)ScreenHeight() / 2 };
	speed = 200.f;
	health = 100.0f;
	fGunReloadTimer = 0.0f;
	fGunReloadDelay = 0.2f;
}


void Player::UpdatePosition(float fElapsedTime) {
	pos.y += (40.0f * fElapsedTime) * 0.5;

	if (GetKey(olc::W).bHeld) pos.y -= speed * fElapsedTime;
	if (GetKey(olc::S).bHeld) pos.y += speed * fElapsedTime;
	if (GetKey(olc::A).bHeld) pos.x -= speed * fElapsedTime;
	if (GetKey(olc::D).bHeld) pos.x += speed * fElapsedTime;

	if (pos.x <= 0) pos.x = 0;
	if (pos.y <= 0) pos.x = 0;

	if (pos.x + 48.0f >= (float)ScreenWidth()) pos.x = (float)ScreenWidth() - 48.0f;
	if (pos.y + 48.0f >= (float)ScreenHeight()) pos.y = (float)ScreenHeight() - 48.0f;
}

void Player::Draw() {
	DrawSprite(pos, sprPlayer);
}