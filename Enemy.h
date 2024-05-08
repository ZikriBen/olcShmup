#ifndef ENEMY_H
#define ENEMY_H
#pragma once
#include "Bullet.h"

struct sEnemy;

struct sEnemyDefiniton {
    double dTriggerTime = 0.0f;
    olc::Sprite* sprEnemy;
    float fHealth = 0.0f;
    float fOffset = 0.0f;

    std::function<void(sEnemy&, float, float)> funcMove;
    std::function<void(sEnemy&, float, float, std::list<sBullet>&)> funcFire;

};

struct sEnemy {
    olc::vf2d pos;
    sEnemyDefiniton def;

    std::array<float, 4> dataMove{0};
    std::array<float, 4> dataFire{0};

    void Update(float fElpasedTime, float fScrollSpeed, std::list<sBullet>& b) {
        def.funcMove(*this, fElpasedTime, fScrollSpeed);
        def.funcFire(*this, fElpasedTime, fScrollSpeed, b);
    }
};

#endif // ENEMY_H
