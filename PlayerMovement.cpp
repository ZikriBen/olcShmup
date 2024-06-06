#include "PlayerMovement.h"
// Johnnug63:
/*-- Private Methods ---*/
void PlayerMovement::ManageSpaceKey(float fElapsedTime)
{
	// Ok we hit the space bar

	if (player.bCanFire) {
		int offsetX = player.listProjectileDef[player.ProjectileType].offsetX;
		int offsetY = player.listProjectileDef[player.ProjectileType].offsetY;

		if (!player.bAutoFire)
			if (player.bSoundOn)
				player.miniAudio.Play(player.souLaserA1);

		if (player.powerUpLevel == 1) {
			Bullet b;
			b.pos = { player.pos.x + offsetX, player.pos.y - offsetY };
			b.vel = { 0.0f, -200.0f };
			player.listPlayerBullets.push_back(b);

		}
		else if (player.powerUpLevel == 2) {
			Bullet b;
			b.pos = { (player.pos.x + offsetX) - 10.f, player.pos.y - offsetY };
			b.vel = { 0.0f, -200.0f };

			player.listPlayerBullets.push_back(b);

			Bullet b2;
			b2.pos = { 10 + (player.pos.x + offsetX), player.pos.y - offsetY };
			b2.vel = { 0.0f, -200.0f };
			player.listPlayerBullets.push_back(b2);
		}
		else {
			Bullet b;
			b.pos = { player.pos.x + offsetX, player.pos.y - offsetY };
			b.vel = { 0.0f, -200.0f };
			player.listPlayerBullets.push_back(b);

			Bullet b2;
			b2.pos = { 10 + (player.pos.x + offsetX), player.pos.y - offsetY };
			b2.vel = { 20.0f, -200.0f };
			player.listPlayerBullets.push_back(b2);
			Bullet b3;
			b3.pos = { (player.pos.x + offsetX) - 10, player.pos.y - offsetY };
			b3.vel = { -20.0f, -200.0f };
			player.listPlayerBullets.push_back(b3);
		}
		player.bCanFire = false;
	}
}


/*-- Public Methods ---*/

PlayerMovement::PlayerMovement(olc::PixelGameEngine& pge, Player& player) : pge(pge), player(player)
{
	// lets get the party started
	int8_t test = 0;  // place holder
}

void PlayerMovement::Update(float fElapsedTime)
{
	if (!bEnable) {
		return;
	}
	// Reset player to standing position
	player.graphicState = Player::STANDING;
	player.facingDirection = Player::NORTH;

	// If W or directional key UP is held
	if (pge.GetKey(olc::Key::W).bHeld || pge.GetKey(olc::Key::UP).bHeld)
	{
		player.graphicState = Player::MOVING;
		player.facingDirection = Player::NORTH;
		player.pos.y -= player.getSpeed() * fElapsedTime;

	}

	// I left this is as you had it in your CommandFactory
	if (pge.GetKey(olc::Key::S).bHeld)
	{
		player.graphicState = Player::MOVING;
		player.facingDirection = Player::NORTH;
		player.pos.y += player.getSpeed() * fElapsedTime;
	}

	// If A or direction key Left is held
	if (pge.GetKey(olc::Key::A).bHeld || pge.GetKey(olc::Key::LEFT).bHeld)
	{
		player.graphicState = Player::MOVING;
		player.facingDirection = Player::WEST;
		player.pos.x -= player.getSpeed() * fElapsedTime;
	}


	// If D or direction key Right is held
	if (pge.GetKey(olc::Key::D).bHeld || pge.GetKey(olc::Key::RIGHT).bHeld)
	{
		player.graphicState = Player::MOVING;
		player.facingDirection = Player::EAST;
		player.pos.x += player.getSpeed() * fElapsedTime;
	}

	// If Z or directional key Down is held
	if (pge.GetKey(olc::Key::Z).bHeld || pge.GetKey(olc::Key::DOWN).bHeld)
	{
		player.graphicState = Player::MOVING;
		player.facingDirection = Player::NORTH;
		player.pos.y += player.getSpeed() * fElapsedTime;
	}

	if (pge.GetKey(olc::Key::SPACE).bPressed)
	{
		if (player.bAutoFire) return;
		// Right we have a bunch of work here, so lets put it in its own private method
		this->ManageSpaceKey(fElapsedTime);
	}

	if (pge.GetKey(olc::Key::Q).bPressed)
	{
		// lets quit


	}

	if (player.bAutoFire) this->ManageSpaceKey(fElapsedTime);
}


