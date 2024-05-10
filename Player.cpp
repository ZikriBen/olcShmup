#include "Player.h"

Player::Player(olc::PixelGameEngine& pge) : pge(pge) {
	sprPlayerSheet = new olc::Sprite("assets/PlayerSpritesheet.png");
	pos = { (float)pge.ScreenWidth() / 2, (float)pge.ScreenHeight() / 2 };
	speed = 200.f;
	health = 3.0f;
	fGunReloadTimer = 0.0f;
	fGunReloadDelay = 0.1f;
	bCanFire = true;
	ih  = new InputHandler(pge);
	graphicState = STANDING;
	facingDirection = NORTH;
	fGraphicTimer = 0.0f;
	graphicCounter = 0;
	fWidth = 48.0f;
	fHeight = 58.0f;
	dead = false;
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
	
	/*if (pge.GetKey(olc::Q).bHeld) {
		for (const auto& tuple : listPlayerCommands) {
			Command* commandPtr = std::get<0>(tuple);
			float time = std::get<1>(tuple);
			char key = commandPtr->getKey();
			std::cout << "Command key: " << commandPtr->getKey() << "time:" << time << std::endl;
		}
	}*/

	Command* command = ih->handleInput();
	if (command)
	{
		command->execute(*this, fElapsedTime);
		delete command; // Release memory
		//listPlayerCommands.push_back(std::tuple < Command*, float>{command, fElapsedTime});
	}
	else {
		graphicState = STANDING;
		facingDirection = NORTH;
	}

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


class buttonW_ : public Command
{
public:
	buttonW_(char key) : Command(key) {}
	virtual void execute(Player& player, float fElapsedTime)
	{
		player.graphicState = Player::MOVING;
		player.facingDirection = Player::NORTH;
		player.pos.y -= player.getSpeed() * fElapsedTime;
	}
};

class buttonS_ : public Command
{
public:
	buttonS_(char key) : Command(key) {}
	virtual void execute(Player& player, float fElapsedTime)
	{
		player.graphicState = Player::MOVING;
		player.facingDirection = Player::NORTH;
		player.pos.y += player.getSpeed() * fElapsedTime;
	}
};

class buttonA_ : public Command
{
public:
	buttonA_(char key) : Command(key) {}
	virtual void execute(Player& player, float fElapsedTime)
	{
		player.graphicState = Player::MOVING;
		player.facingDirection = Player::WEST;
		player.pos.x -= player.getSpeed() * fElapsedTime;

	}
};

class buttonD_ : public Command
{
public:
	buttonD_(char key) : Command(key) {}
	virtual void execute(Player& player, float fElapsedTime)
	{
		player.graphicState = Player::MOVING;
		player.facingDirection = Player::EAST;
		player.pos.x += player.getSpeed() * fElapsedTime;
	}
};

class buttonSPACE_ : public Command
{
public:
	buttonSPACE_(char key) : Command(key) {}
	virtual void execute(Player& player, float fElapsedTime)
	{
		if (player.bCanFire) {
			Bullet b;
			b.pos = { player.pos.x + ((float)player.getWidth() / 2.0f), player.pos.y };
			b.vel = { 0.0f, -200.0f };
			player.listPlayerBullets.push_back(b);
			player.bCanFire = false;
		}
	}
};

Command *InputHandler::handleInput()
{
	if (pge.GetKey(olc::W).bHeld) return new buttonW_('w');
	if (pge.GetKey(olc::S).bHeld) return new buttonS_('s');
	if (pge.GetKey(olc::A).bHeld) return new buttonA_('a');
	if (pge.GetKey(olc::D).bHeld) return new buttonD_('d');
	if (pge.GetKey(olc::SPACE).bHeld) return new buttonSPACE_(' ');

	return nullptr;
}



