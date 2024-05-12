#include "Bullet.h"
#include "Player.h"


void Bullet::Update(const float fElapsedTime, const float fWorldSpeed, Player &player) {
    pos += (vel + olc::vf2d(0.0f, fWorldSpeed)) * fElapsedTime;
    if ((pos - (player.pos + olc::vf2d(((float)player.getWidth() / 2.0f), ((float)player.getWidth() / 2.0f)))).mag2() < powf(((float)player.getWidth() / 2.0f), 2.0f)) {
        remove = true;
        player.setHealth(player.getHealth() - 1.0f);
    }
}