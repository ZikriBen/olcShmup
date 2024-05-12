#ifndef ENEMY_H
#define ENEMY_H
#include "Spawn.h"

#pragma once

struct sEnemy : public sSpawn {
    
    sEnemy(): sSpawn() {};
    sEnemy(sEnemyDefiniton *def) : def(def) {};
    sEnemyDefiniton* def;

    

    void Update(float fElpasedTime, float fScrollSpeed, std::list<Bullet>& b) {
        def->funcMove(*this, fElpasedTime, fScrollSpeed);
        def->funcFire(*this, fElpasedTime, fScrollSpeed, b);
    }
    virtual void dummy() const {}
};

#endif // ENEMY_H
