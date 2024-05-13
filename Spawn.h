#ifndef SPAWN_H
#define SPAWN_H
#include "olcPixelGameEngine.h"
#include "Bullet.h"

#pragma once

enum class SpawnType {
    SPAWN,
    ENEMY,
    POWERUP
};

enum class powerUpType {
    DEFAULT = 0,
    GREEN = 1,
    BLUE = 2
};

struct sEnemy;

class sSpawn {
public:
    sSpawn() : pos({ 0.0f, 0.0f }), velocity({ 0.0f, 0.0f }), fWidth(0.0f), fHeight(0.0f), fSpeed(120.0f){
        velocity = { fSpeed, fSpeed };
    };
    olc::vf2d pos;
    std::array<float, 4> dataMove{0};
    std::array<float, 4> dataFire{0};
    olc::vf2d velocity;
    
    float fWidth, fHeight;
    float fSpeed;
    bool remove = false;
    bool bBlink = false;
    virtual void dummy() const {}
};

class Spawn
{
public:
    Spawn() : dTriggerTime(0.0f), spr(nullptr), fOffset(0.0f), type(SpawnType::SPAWN), funcMove(nullptr), funcFire(nullptr) {}
    Spawn(double dTriggerTime, olc::Sprite* spr, float fOffset, SpawnType type, std::function<void(sSpawn&, float, float)> funcMove, std::function<void(sSpawn&, float, float, std::list<Bullet>&)> funcFire) :
        dTriggerTime(dTriggerTime), spr(spr), fOffset(fOffset), type(type), funcMove(funcMove), funcFire(funcFire) {}
    double dTriggerTime = 0.0f;
    olc::Sprite* spr;
    float fOffset = 0.0f;
    SpawnType type;
    std::function<void(sSpawn&, float, float)> funcMove;
    std::function<void(sSpawn&, float, float, std::list<Bullet>&)> funcFire;
    virtual void dummy() const {}
};

class sEnemyDefiniton : public Spawn {
public:
    float fHealth = 0.0f;
    
    sEnemyDefiniton() : Spawn(0.0f, nullptr, 0.0f, SpawnType::ENEMY, nullptr, nullptr), fHealth(0.0f) {}
    
    sEnemyDefiniton(double triggerTime, olc::Sprite* spr, float offset, std::function<void(sSpawn&, float, float)> funcMove, std::function<void(sSpawn&, float, float, std::list<Bullet>&)> funcFire, float health)
        : Spawn{ triggerTime, spr, offset, SpawnType::ENEMY, funcMove, funcFire }, fHealth(health)
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
    powerUpType type;

    sPowerUpDefiniton() : Spawn(0.0f, nullptr, 0.0f, SpawnType::POWERUP, nullptr, nullptr), type(powerUpType::DEFAULT) {
        
    }

    sPowerUpDefiniton(double triggerTime, olc::Sprite* spr, float offset, std::function<void(sSpawn&, float, float)> funcMove, std::function<void(sSpawn&, float, float, std::list<Bullet>&)> funcFire, powerUpType type)
        : Spawn{ triggerTime, spr, offset, SpawnType::POWERUP, funcMove, funcFire}, type(type)
    {}

    sPowerUpDefiniton& operator=(const sPowerUpDefiniton& other) {
        if (this != &other) {
            dTriggerTime = other.dTriggerTime;
            spr = other.spr;
            fOffset = other.fOffset;
            funcMove = other.funcMove;
            funcFire = other.funcFire;
            type = other.type;
        }
        return *this;
    }
};

#endif // SPAWN_H