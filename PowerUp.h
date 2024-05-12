
#include "olcPixelGameEngine.h" 
#include "Spawn.h"

class PowerUp {
public:
    PowerUp(olc::PixelGameEngine& pge, olc::Sprite* spr, float x, float y, float speed, float fTriggerTime) : pge(pge), sprite(spr), position({ x, y }), fSpeed(speed), fTriggerTime(fTriggerTime) {
        fWidth = static_cast<float>(sprite->width);
        fHeight = static_cast<float>(sprite->height);
        velocity = { fSpeed, fSpeed };
    }

    void Update(float fElapsedTime) {
        // Update position based on velocity
        position += velocity * fElapsedTime;

        // Check collision with screen boundaries
        if (position.x < 0) {
            position.x = 0;
            velocity.x *= -1; 
        }
        else if (position.x + fWidth > pge.ScreenWidth()) {
            position.x = pge.ScreenWidth() - fWidth;
            velocity.x *= -1; 
        }

        if (position.y < 0) {
            position.y = 0;
            velocity.y *= -1;
        }
        else if (position.y + fHeight > pge.ScreenHeight()) {
            position.y = pge.ScreenHeight() - fHeight;
            velocity.y *= -1; 
        }
    }

    void Draw() {
        pge.DrawSprite(position.x, position.y, sprite);
    }

    float fTriggerTime;

private:
    olc::PixelGameEngine& pge;
    olc::Sprite* sprite;
    olc::vf2d position;
    olc::vf2d velocity;
    sPowerUpDefiniton def;
    float fWidth, fHeight;
    float fSpeed;
};

