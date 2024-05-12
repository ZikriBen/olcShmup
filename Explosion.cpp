#include "Explosion.h"

void Explosion::Start() {
	fGraphicTimer = 0.0f;
	graphicCounter = 0;
}

void Explosion::Update(float fElapsedTime, Player &player)
{
	if (player.lifeState == Player::DYING) {
		
		fGraphicTimer += fElapsedTime;
		if (fGraphicTimer > 0.045) {
			fGraphicTimer -= 0.045f;
			graphicCounter++;
		}
		if (graphicCounter > frames) {
			remove = true;
			player.lifeState = Player::DEAD;
		}
	}
}

void Explosion::Draw()
{
	int nSheetOffsetX = 0;
	nSheetOffsetX = graphicCounter * width;
	pge.SetPixelMode(olc::Pixel::MASK);
	pge.DrawPartialSprite(pos.x, pos.y, spriteSheet, nSheetOffsetX, 0, width, height, 1, 0);
	pge.SetPixelMode(olc::Pixel::NORMAL);
}
