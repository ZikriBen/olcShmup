
#include "olcPixelGameEngine.h" 
#include "Spawn.h"
#include "Enemy.h"


class PowerUp : public sSpawn {
public:
    float fTriggerTime;
    sPowerUpDefiniton* def;
    olc::Sprite* sprite;
    

    PowerUp(): sSpawn() {};
    PowerUp(sPowerUpDefiniton* def) : def(def) {};
    PowerUp(olc::PixelGameEngine& pge, olc::Sprite* spr, float x, float y, float speed, float fTriggerTime) : sprite(spr), fTriggerTime(fTriggerTime) {
    }

    void Update(float fElapsedTime, float fScrollSpeed) {
        def->funcMove(*this, fElapsedTime, fScrollSpeed);
    }
};

