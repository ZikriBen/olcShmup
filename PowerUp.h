
#include "olcPixelGameEngine.h" 
#include "Spawn.h"
#include "Enemy.h"


class PowerUp : public sSpawn {
public:
    float fTriggerTime;
    sPowerUpDefiniton* def;
    olc::Sprite* sprite;
    float fBlinkDelay = 8;
    float fBlinkTimer = 0.0f;
    int blinkCounter = 20;
    float blinkInterval = 0.2;
    float fGraphicTimer = 0.0f;
    int graphicCounter = 0;
 

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
        fGraphicTimer += fElapsedTime;
        if (fGraphicTimer > 0.1f) {
            fGraphicTimer -= 0.1f;
            graphicCounter++;
            graphicCounter %= 3;
        }
    }

    void Draw(olc::PixelGameEngine& pge) {

        pge.SetPixelMode(olc::Pixel::MASK);
        pge.DrawPartialSprite(pos.x, pos.y, def->spr, graphicCounter * fWidth, 0, fWidth, fHeight, 1, 0);
    }
};

