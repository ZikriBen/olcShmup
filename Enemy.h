#ifndef ENEMY_H
#define ENEMY_H
#include "Spawn.h"

#pragma once

struct sEnemy : public sSpawn {
    
    sEnemy(): sSpawn() {};
    sEnemy(sEnemyDefinition*def) : def(def) {};
    sEnemyDefinition* def;

    virtual void Update(float fElpasedTime, float fScrollSpeed, std::list<Bullet>& b) {
        def->funcMove(*this, fElpasedTime, fScrollSpeed);
        def->funcFire(*this, fElpasedTime, fScrollSpeed, b);
    }
    virtual void dummy() const {}
};

struct sBoss : public sEnemy {
    sBoss() : sEnemy() {}
    sBoss(sEnemyDefinition* def) : sEnemy(def) {}

    virtual void Update(float fElapsedTime, float fScrollSpeed, std::list<Bullet>& b) override {
        // Implement boss-specific update logic here
        sEnemy::Update(fElapsedTime, fScrollSpeed, b); // Call base class update method
    }

    // Add any additional methods or attributes specific to bosses here
};

#endif // ENEMY_H
