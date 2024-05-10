#include "Enemy.h"

void sEnemy::Update(float fElpasedTime, float fScrollSpeed, std::list<Bullet>& b) {
    def.funcMove(*this, fElpasedTime, fScrollSpeed);
    def.funcFire(*this, fElpasedTime, fScrollSpeed, b);
}