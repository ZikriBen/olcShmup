#include "Player.h"

Player::Player(olc::PixelGameEngine& pge) : pge(pge) {
	sprPlayer = new olc::Sprite("assets/playerShip.png");
	pos = { (float)pge.ScreenWidth() / 2, (float)pge.ScreenHeight() / 2 };
	speed = 200.f;
	health = 100.0f;
	fGunReloadTimer = 0.0f;
	fGunReloadDelay = 0.2f;
	bCanFire = true;
	ih  = new InputHandler(pge);
}


void Player::UpdatePosition(float fElapsedTime) {
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

	listPlayerBullets.remove_if([&](const sBullet& b) {return b.pos.x<0 || b.pos.x>pge.ScreenWidth() || b.pos.y <0 || b.pos.y>pge.ScreenHeight() || b.remove; });
}

void Player::Draw() {
	pge.DrawSprite(pos, sprPlayer);
}

float Player::getWidth() {
	return sprPlayer->width;
}

float Player::getHeight() {
	return sprPlayer->height;
}

class buttonW_ : public Command
{
public:
	buttonW_(char key) : Command(key) {}
	virtual void execute(Player& player, float fElapsedTime)
	{
		player.pos.y -= player.speed * fElapsedTime;
	}
};

class buttonS_ : public Command
{
public:
	buttonS_(char key) : Command(key) {}
	virtual void execute(Player& player, float fElapsedTime)
	{
		player.pos.y += player.speed * fElapsedTime;
	}
};

class buttonA_ : public Command
{
public:
	buttonA_(char key) : Command(key) {}
	virtual void execute(Player& player, float fElapsedTime)
	{
		player.pos.x -= player.speed * fElapsedTime;

	}
};

class buttonD_ : public Command
{
public:
	buttonD_(char key) : Command(key) {}
	virtual void execute(Player& player, float fElapsedTime)
	{
		player.pos.x += player.speed * fElapsedTime;
	}
};

class buttonSPACE_ : public Command
{
public:
	buttonSPACE_(char key) : Command(key) {}
	virtual void execute(Player& player, float fElapsedTime)
	{
		if (player.bCanFire) {
			sBullet b;
			b.pos = { player.pos.x + 24.0f, player.pos.y };
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



