#ifndef SPAWN_H
#define SPAWN_H
#include "olcPixelGameEngine.h"
#include "Bullet.h"

#pragma once

enum class SpawnType {
    ENEMY,
    POWERUP
};

struct sEnemy;

class Spawn
{
public:
    olc::Sprite* spr;
    double dTriggerTime = 0.0f;
    float fOffset = 0.0f;
    SpawnType type;
    std::function<void(sEnemy&, float, float)> funcMove;
    std::function<void(sEnemy&, float, float, std::list<Bullet>&)> funcFire;
};

class sEnemyDefiniton : public Spawn {
public:
    float fHealth = 0.0f;
    SpawnType type = SpawnType::ENEMY;
    
    sEnemyDefiniton() {
        dTriggerTime = 0.0f;
        spr = nullptr;
        fHealth = 0.0f;
        fOffset = 0.0f;
        funcMove = nullptr;
        funcFire = nullptr;
    }
    
    sEnemyDefiniton(double triggerTime, olc::Sprite* spr, float health, float offset, std::function<void(sEnemy&, float, float)> funcMove, std::function<void(sEnemy&, float, float, std::list<Bullet>&)> funcFire)
        : Spawn{ spr, triggerTime, offset, SpawnType::ENEMY, funcMove, funcFire }, fHealth(health)
    {}
    
    sEnemyDefiniton& operator=(const sEnemyDefiniton& other) {
        if (this != &other) { 
            dTriggerTime = other.dTriggerTime;
            spr = other.spr;
            fHealth = other.fHealth;
            fOffset = other.fOffset;
            funcMove = other.funcMove;
            funcFire = other.funcFire;
        }
        return *this;
    }
};

class sPowerUpDefiniton : public Spawn {
public:
    sPowerUpDefiniton(double triggerTime, olc::Sprite* spr,  float offset, std::function<void(sEnemy&, float, float)> funcMove, std::function<void(sEnemy&, float, float, std::list<Bullet>&)> funcFire)
        : Spawn{ spr, triggerTime, offset, SpawnType::ENEMY, funcMove, funcFire }
    {}
    sPowerUpDefiniton() {
        dTriggerTime = 0.0f;
        spr = nullptr;
        fOffset = 0.0f;
        funcMove = nullptr;
        funcFire = nullptr;
    }
};

#endif // SPAWN_H