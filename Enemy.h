#ifndef ENEMY_H
#define ENEMY_H
#include "Spawn.h"

#pragma once

struct sEnemy {
    olc::vf2d pos;
    sEnemyDefiniton def;

    std::array<float, 4> dataMove{0};
    std::array<float, 4> dataFire{0};

    void Update(float fElpasedTime, float fScrollSpeed, std::list<Bullet>& b);
};

#endif // ENEMY_H
