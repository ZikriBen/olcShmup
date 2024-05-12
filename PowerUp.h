
#include "olcPixelGameEngine.h" 
#include "Spawn.h"
#include "Enemy.h"


class PowerUp : public sSpawn {
public:
    float fTriggerTime;
    sPowerUpDefiniton* def;
    olc::Sprite* sprite;
    float fBlinkDelay = 3;
    float fBlinkTimer = 0.0f;
    int blinkCounter = 20;
    float blinkInterval = 0.2;    

    PowerUp(): sSpawn() {};
    PowerUp(sPowerUpDefiniton* def) : def(def) {};
    PowerUp(olc::PixelGameEngine& pge, olc::Sprite* spr, float x, float y, float speed, float fTriggerTime) : sprite(spr), fTriggerTime(fTriggerTime) {
    }

    void Update(float fElapsedTime, float fScrollSpeed) {
        def->funcMove(*this, fElapsedTime, fScrollSpeed);
        fBlinkDelay -= fElapsedTime;
        if (fBlinkDelay > fElapsedTime) {
            bBlink = true;
        }
        else {
            fBlinkTimer += fElapsedTime;
            if (fBlinkTimer > blinkInterval) {
                fBlinkTimer -= blinkInterval;
                bBlink = !bBlink;
                blinkCounter--;
            }
            if (!blinkCounter) {
                remove = true;
            }
        }
    }
};

