//// olcShmup.cpp : This file contains the 'main' function. Program execution begins and ends there.
////
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

constexpr double PI = 3.14159f;

class Shmup : public olc::PixelGameEngine {
public:
    Shmup() {
        sAppName = "Shmup";
    }

    olc::vf2d vPlayerPos;
    olc::Sprite* sprPlayer;
    std::array<olc::Sprite*, 3> sprEnemy;

    float fPlayerSpeed = 200.f;
    float fPlayerHealth = 100.0f;
    float fWorldSpeed = 40.0f;
    double dWorldPos = 0;
    float fGunReloadTimer = 0.0f;
    float fGunReloadDelay = 0.2f;

    std::array<olc::vf2d, 800> arrStars;
    struct sEnemy;
    
    struct sBullet {
        olc::vf2d pos;
        olc::vf2d vel;
        bool remove = false;
    };

    struct sEnemyDefiniton {
        double dTriggerTime = 0.0f;
        uint32_t nSpriteID = 0;
        float fHealth = 0.0f;
        float fOffset = 0.0f;

        std::function<void(sEnemy&, float, float)> funcMove;
        std::function<void(sEnemy&, float, float, std::list<sBullet>&)> funcFire;

    };

    struct sEnemy {
        olc::vf2d pos;
        sEnemyDefiniton def;

        std::array<float, 4> dataMove{0};
        std::array<float, 4> dataFire{0};

        void Update(float fElpasedTime, float fScrollSpeed, std::list<sBullet> &b) {
            def.funcMove(*this, fElpasedTime, fScrollSpeed);
            def.funcFire(*this, fElpasedTime, fScrollSpeed, b);
        }
    };

    bool bCanFire = false;
    std::list<sEnemyDefiniton> listSpawns;
    std::list<sEnemy> listEnemies;
    std::list<sBullet> listBullets;
    std::list<sBullet> listPlayerBullets;
    std::list<sBullet> listFragments;

public:
    bool OnUserCreate() override {
        sprPlayer = new olc::Sprite("assets//playerShip.png");
        sprEnemy[0] = new olc::Sprite("assets//enemyShip01.png");
        sprEnemy[1] = new olc::Sprite("assets//enemyShip01.png");
        sprEnemy[2] = new olc::Sprite("assets//enemyShip01.png");
        vPlayerPos = { (float)ScreenWidth() / 2, (float)ScreenHeight() / 2 };

        for (auto& star : arrStars) star = {(float)(rand() % ScreenWidth()), (float)(rand() % ScreenHeight())};

        // Movement Patterns
        auto Move_None = [&](sEnemy& e, float fElapsedTime, float fScrollSpeed) {
            e.pos.y += fScrollSpeed * fElapsedTime;
        };

        auto Move_Fast = [&](sEnemy& e, float fElapsedTime, float fScrollSpeed) {
            e.pos.y += fScrollSpeed * fElapsedTime * 3.0f;
            
        };

        auto Move_SinusoidNarrow = [&](sEnemy& e, float fElapsedTime, float fScrollSpeed) {
            e.pos.y += fScrollSpeed * fElapsedTime * 1.0f;
            e.dataMove[0] += fElapsedTime;
            e.pos.x += 50.0f * cosf(e.dataMove[0]) * fElapsedTime;
        };

        auto Move_SinusoidWide = [&](sEnemy& e, float fElapsedTime, float fScrollSpeed) {
            e.pos.y += fScrollSpeed * fElapsedTime * 1.0f;
            e.dataMove[0] += fElapsedTime;
            e.pos.x += 200.0f * cosf(e.dataMove[0]) * fElapsedTime;
        };

        auto Fire_None = [&](sEnemy& e, float fElapsedTime, float fScrollSpeed, std::list<sBullet>& b) {};

        auto Fire_Straigt2 = [&](sEnemy& e, float fElapsedTime, float fScrollSpeed, std::list<sBullet>& b) {
            constexpr float fDelay = 0.2f;
            e.dataFire[0] += fElapsedTime;
            if (e.dataFire[0] >= fDelay) {
                e.dataFire[0] -= fDelay;
                sBullet b;
                b.pos = e.pos + (olc::vf2d(24, 48)); // Remove magic numbers!
                b.vel = { 0.0f, 180.0f };
                listBullets.push_back(b);
            }
        };

        auto Fire_CirclePulse2 = [&](sEnemy& e, float fElapsedTime, float fScrollSpeed, std::list<sBullet>& b) {
            constexpr float fDelay = 2.0f;
            constexpr int nBullets = 10;
            constexpr float fTetha = PI * 2.0f / ((float)nBullets);

            e.dataFire[0] += fElapsedTime;
            if (e.dataFire[0] >= fDelay) {
                e.dataFire[0] -= fDelay;
                for (int i = 0; i < nBullets; ++i) {
                    sBullet b;
                    b.pos = e.pos + (olc::vf2d(24, 24)); // Remove magic numbers!
                    b.vel = { 180.0f * cosf(fTetha * i), 180.0f * sinf(fTetha * i) };
                    listBullets.push_back(b);
                }
            }
        };

        auto Fire_DeathSpiral = [&](sEnemy& e, float fElapsedTime, float fScrollSpeed, std::list<sBullet>& b) {
            constexpr float fDelay = 0.01f;
            constexpr int nBullets = 50;
            constexpr float fTetha = PI * 2.0f / ((float)nBullets);

            e.dataFire[0] += fElapsedTime;
            if (e.dataFire[0] >= fDelay) {
                e.dataFire[0] -= fDelay;
                e.dataFire[1] += 0.1f;

                sBullet b;
                b.pos = e.pos + (olc::vf2d(24, 24)); // Remove magic numbers!
                b.vel = { 180.0f * cosf(e.dataFire[1]), 180.0f * sinf(e.dataFire[1])};
                listBullets.push_back(b);
            }
        };



        listSpawns = {
            {60.00, 0, 3.0f, 0.5f, Move_SinusoidWide, Fire_CirclePulse2},
            {240.0, 0, 3.0f, 0.25f, Move_SinusoidNarrow, Fire_Straigt2},
            {240.0, 0, 3.0f, 0.75f, Move_SinusoidNarrow, Fire_Straigt2},
            {360.0, 0, 3.0f, 0.2f, Move_None, Fire_Straigt2},
            {360.0, 0, 3.0f, 0.5f, Move_None, Fire_None},
            {360.0, 0, 3.0f, 0.8f, Move_None, Fire_Straigt2},
            {500, 0, 3.0f, 0.5f, Move_Fast, Fire_DeathSpiral}
        };

        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {
        
        vPlayerPos.y += (fWorldSpeed * fElapsedTime) * 0.5;

        //INPUT HANDLING
        if (GetKey(olc::W).bHeld) vPlayerPos.y -= fPlayerSpeed * fElapsedTime;
        if (GetKey(olc::S).bHeld) vPlayerPos.y += fPlayerSpeed * fElapsedTime;
        if (GetKey(olc::A).bHeld) vPlayerPos.x -= fPlayerSpeed * fElapsedTime;
        if (GetKey(olc::D).bHeld) vPlayerPos.x += fPlayerSpeed * fElapsedTime;
        
        if (vPlayerPos.x <= 0) vPlayerPos.x = 0;
        if (vPlayerPos.y <= 0) vPlayerPos.x = 0;
        
        if (vPlayerPos.x + 48.0f >= (float)ScreenWidth()) vPlayerPos.x = (float)ScreenWidth() - 48.0f;
        if (vPlayerPos.y + 48.0f >= (float)ScreenHeight()) vPlayerPos.y = (float)ScreenHeight() - 48.0f;
            


        fGunReloadTimer += fElapsedTime;

        if (fGunReloadTimer >= fGunReloadDelay) {
            bCanFire = true;
            fGunReloadTimer -= fGunReloadDelay;
        }

        if (GetKey(olc::SPACE).bHeld || GetMouse(0).bHeld) {
            if (bCanFire) {
                sBullet b;
                b.pos = { vPlayerPos.x + 24.0f, vPlayerPos.y };
                b.vel = { 0.0f, -200.0f };
                listPlayerBullets.push_back(b);
                bCanFire = false;
            }
        }

        //GUBBINS
        dWorldPos += fWorldSpeed * fElapsedTime;

        while (!listSpawns.empty() && dWorldPos >= listSpawns.front().dTriggerTime) {
            sEnemy e;
            e.def = listSpawns.front();
            e.pos = {
                e.def.fOffset * ((float)ScreenWidth()) - (((float)sprEnemy[e.def.nSpriteID]->width) / 2),
                0.0f - ((float)sprEnemy[e.def.nSpriteID]->height)
            };
            listSpawns.pop_front();
            listEnemies.push_back(e);
        }

        //DRAW
        Clear(olc::BLACK);

        for (size_t i = 0; i < arrStars.size(); ++i) {
            auto& star = arrStars[i];

            star.y += fWorldSpeed * fElapsedTime * ((i < 200) ? 0.8f : 1.0f);
            if (star.y > (float)ScreenHeight()) {
                star = { (float)(rand() % ScreenWidth()), 0.0f };
            }
            Draw(star, ((i < 200) ? olc::DARK_GREY : olc::WHITE));
        }
        
        //UPDATE

        for (auto& b : listFragments) {
            b.pos += (b.vel + olc::vd2d(0.0f, fWorldSpeed)) * fElapsedTime;
        }

        for (auto& e : listEnemies) e.Update(fElapsedTime, fWorldSpeed, listBullets);
        for (auto& b : listBullets) {
            b.pos += (b.vel + olc::vd2d(0.0f, fWorldSpeed)) * fElapsedTime;
            if ((b.pos - (vPlayerPos + olc::vd2d(24, 24))).mag2() < 24.0f * 24.0f) { // Remove magic numbers!
                b.remove = true;
                fPlayerHealth -= 1.0f;
            }
        }

        for (auto& b : listPlayerBullets) {
            b.pos += (b.vel + olc::vd2d(0.0f, fWorldSpeed)) * fElapsedTime;
            for (auto& e : listEnemies) {
                if ((b.pos - (e.pos + olc::vd2d(24, 24))).mag2() < 24.0f * 24.0f) { // Remove magic numbers!
                    b.remove = true;
                    e.def.fHealth -= 1.0f;
                    if (e.def.fHealth <= 0) {
                        for (int i = 0; i < 500; ++i) {
                            float fAngle = ((float)rand() / (float)RAND_MAX) * 2.0f * PI;
                            float fSpeed = ((float)rand() / (float)RAND_MAX) * 200.0f + 50.0f;
                            listFragments.push_back({ e.pos + olc::vf2d(24,24), {fSpeed * cosf(fAngle), fSpeed * sinf(fAngle)}});
                        }
                    }
                }
            }
            
        }

        listEnemies.remove_if([&](const sEnemy& e) {return e.pos.y >= ((float)ScreenHeight()) || e.def.fHealth <= 0; });

        listBullets.remove_if([&](const sBullet& b) {return b.pos.x < 0 || b.pos.x>ScreenWidth() || b.pos.y <0 || b.pos.y>ScreenHeight() || b.remove; });
        
        listPlayerBullets.remove_if([&](const sBullet& b) {return b.pos.x<0 || b.pos.x>ScreenWidth() || b.pos.y <0 || b.pos.y>ScreenHeight() || b.remove; });
        listFragments.remove_if([&](const sBullet& b) {return b.pos.x<0 || b.pos.x>ScreenWidth() || b.pos.y <0 || b.pos.y>ScreenHeight() || b.remove; });
       
        SetPixelMode(olc::Pixel::MASK);
        DrawSprite(vPlayerPos, sprPlayer);
        
        for (auto& e : listEnemies) DrawSprite(e.pos, sprEnemy[e.def.nSpriteID], 1, olc::Sprite::Flip::VERT);

        for (auto& b : listBullets) FillCircle(b.pos, 3, olc::RED);

        for (auto& b : listPlayerBullets) FillCircle(b.pos, 3, olc::CYAN);
        for (auto& b : listFragments) Draw(b.pos, olc::YELLOW);

        //HUD
        DrawString(4, 4, "HEALTH:");
        FillRect(60, 4, (fPlayerHealth / 100.0f * 576.0f), 8, olc::GREEN); // Remove magic numbers!
        
        SetPixelMode(olc::Pixel::NORMAL);

        return true;
    }
};


int main()
{
	Shmup game;
	if (game.Construct(640, 480, 2, 2))
		game.Start();

	return 0;
}


// Optimize stars to initialization
// add sprite stars/nebulas etc
// add main menu
// convert input handling to command design pattern
// make player ship not go out of bounds
// make the dataMove and dataFire as a calss member
// Remove magic numbers!
// add explosion sprite
// add score
// particle fade away
// add player death