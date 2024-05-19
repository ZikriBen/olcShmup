#ifndef ENEMY_H
#define ENEMY_H
#include "Spawn.h"

#pragma once

enum class BossDamageLevel {
    NO_DAMAGE = 0,
    MINOR_DAMAGE = 1,
    MODERATE_DAMAGE = 2,
    SEVERE_DAMAGE = 3,
    CRITICAL_DAMAGE = 4,
    DEAD = 5
};

struct sEnemy : public sSpawn {
    sEnemy(): sSpawn() {};
    sEnemy(sEnemyDefinition*def) : def(def) {};
    sEnemyDefinition* def;

    virtual void Update(float fElpasedTime, float fScrollSpeed, std::list<Bullet>& b) {
        def->funcMove(*this, fElpasedTime, fScrollSpeed);
        def->funcFire(*this, fElpasedTime, fScrollSpeed, b);
    }

    virtual void Draw(olc::PixelGameEngine& pge) {
        pge.DrawSprite(pos, def->spr, 1, olc::Sprite::Flip::VERT);
    }
    virtual void dummy() const {}
};

struct sBoss : public sEnemy {
    sBoss() : sEnemy() {}
    sBoss(sEnemyDefinition* def) : sEnemy(def) {}
    BossDamageLevel damageLevel = BossDamageLevel::NO_DAMAGE;

    virtual void Update(float fElapsedTime, float fScrollSpeed, std::list<Bullet>& b) override {
        sEnemy::Update(fElapsedTime, fScrollSpeed, b); // Call base class update method
        if (def->fHealth == 3) {
            damageLevel = BossDamageLevel::NO_DAMAGE;
        }
        else if (def->fHealth == 2) {
            damageLevel = BossDamageLevel::MODERATE_DAMAGE;
        }
        else {
            damageLevel = BossDamageLevel::CRITICAL_DAMAGE;
        }
    }

    virtual void Draw(olc::PixelGameEngine& pge) override {
        pge.DrawPartialSprite(pos.x, pos.y, def->spr, (182 * static_cast<int>(damageLevel)), 0, 182, 122, 1, 0);
    }
};

#endif // ENEMY_H
