#ifndef SPAWN_H
#define SPAWN_H

#include "olcPixelGameEngine.h"
#include "Bullet.h"
#include <functional>
#include <list>

enum class SpawnType {
    SPAWN,
    ENEMY,
    POWERUP,
    BOSS
};

enum class PowerUpType {
    DEFAULT = 0,
    GREEN = 1,
    BLUE = 2
};



struct sEnemy;

class sSpawn {
public:
    sSpawn() : pos({ 0.0f, 0.0f }), velocity({ 0.0f, 0.0f }), fWidth(0), fHeight(0), fSpeed(120.0f), remove(false), bBlink(false) {
        velocity = { fSpeed, fSpeed };
    }
    olc::vf2d pos;
    std::array<float, 4> dataMove{0};
    std::array<float, 4> dataFire{0};
    olc::vf2d velocity;

    int fWidth, fHeight;
    float fSpeed;
    bool remove;
    bool bBlink;
    virtual void dummy() const {}
};

class Spawn {
public:
    Spawn() : dTriggerTime(0.0f), spr(nullptr), fOffset(0.0f), type(SpawnType::SPAWN), funcMove(nullptr), funcFire(nullptr) {}
    Spawn(double dTriggerTime, olc::Sprite* spr, float fOffset, SpawnType type, std::function<void(sSpawn&, float, float)> funcMove, std::function<void(sSpawn&, float, float, std::list<Bullet>&)> funcFire) :
        dTriggerTime(dTriggerTime), spr(spr), fOffset(fOffset), type(type), funcMove(funcMove), funcFire(funcFire) {}
    double dTriggerTime;
    olc::Sprite* spr;
    float fOffset;
    SpawnType type;
    std::function<void(sSpawn&, float, float)> funcMove;
    std::function<void(sSpawn&, float, float, std::list<Bullet>&)> funcFire;
    virtual void dummy() const {}
};

class sEnemyDefinition : public Spawn {
public:
    float fHealth;
    int iWidth;
    int iHeight;

    sEnemyDefinition() : Spawn(0.0f, nullptr, 0.0f, SpawnType::ENEMY, nullptr, nullptr), fHealth(0.0f), iWidth(0), iHeight(0) {}

    sEnemyDefinition(double triggerTime, olc::Sprite* spr, float offset, std::function<void(sSpawn&, float, float)> funcMove, std::function<void(sSpawn&, float, float, std::list<Bullet>&)> funcFire, float health, int width, int height)
        : Spawn(triggerTime, spr, offset, SpawnType::ENEMY, funcMove, funcFire), fHealth(health), iWidth(width), iHeight(height)
    {}
};

class sPowerUpDefinition : public Spawn {
public:
    PowerUpType type;

    sPowerUpDefinition() : Spawn(0.0f, nullptr, 0.0f, SpawnType::POWERUP, nullptr, nullptr), type(PowerUpType::DEFAULT) {}

    sPowerUpDefinition(double triggerTime, olc::Sprite* spr, float offset, std::function<void(sSpawn&, float, float)> funcMove, std::function<void(sSpawn&, float, float, std::list<Bullet>&)> funcFire, PowerUpType type)
        : Spawn(triggerTime, spr, offset, SpawnType::POWERUP, funcMove, funcFire), type(type)
    {}
};

struct sBossDefiniton : public sEnemyDefinition {
    sBossDefiniton() : sEnemyDefinition() {
        type = SpawnType::BOSS;  
    }

    sBossDefiniton(double triggerTime, olc::Sprite* spr, float offset, std::function<void(sSpawn&, float, float)> funcMove, std::function<void(sSpawn&, float, float, std::list<Bullet>&)> funcFire, float health, int width, int height)
        : sEnemyDefinition(triggerTime, spr, offset, funcMove, funcFire, health, width, height){
        type = SpawnType::BOSS;
    }
};

#endif // SPAWN_H
