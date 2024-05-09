#include "Background.h"

Background::Background(olc::PixelGameEngine& pge, float fWorldSpeed, int nFarStars) : pge(pge), fWorldSpeed(fWorldSpeed), nFarStars(nFarStars), fNebulaeSpacer(1.5f), fFarStarsSpeed(0.2f){
}

void Background::populateStart(){
    for (auto& star : arrStars) 
        star = { (float)(rand() % pge.ScreenWidth()), (float)(rand() % pge.ScreenHeight()) };

    olc::Sprite* nebulaSpr = new olc::Sprite("assets/nebula_1.png");
    olc::vf2d artPos = { (float)(rand() % (pge.ScreenWidth() - (2 * nebulaSpr->width))), (float)(rand() % (pge.ScreenHeight() - nebulaSpr->height)) };
    Artifact *nebula1 = new Artifact(artPos, nebulaSpr, 1, 1);

    nebulas.push_back(nebula1);
}

void Background::Draw() {
    for (size_t i = 0; i < arrStars.size(); ++i) {
        auto& star = arrStars[i];
        pge.Draw(star, ((i < nFarStars) ? olc::DARK_GREY : olc::WHITE));
    }
    for (auto& nebula : nebulas) {
        pge.DrawSprite(nebula->pos, nebula->spr, nebula->scale, nebula->flip);
    }
}

void Background::Update(float fElapsedTime) {

    for (size_t i = 0; i < arrStars.size(); ++i) {
        auto& star = arrStars[i];

        star.y += fWorldSpeed * fElapsedTime * ((i < nFarStars) ? 1.0f - fFarStarsSpeed : 1.0f);
        if (star.y > (float)pge.ScreenHeight()) {
            star = { (float)(rand() % pge.ScreenWidth()), 0.0f };
        }
    }

    for (auto& nebula : nebulas) {
        nebula->pos.y += fWorldSpeed * fElapsedTime;
        if (nebula->pos.y > (float)pge.ScreenHeight()) {
            nebula->scale = (rand() % 2) + 1;
            nebula->flip = rand() % 3;
            nebula->pos = { (float)(rand() % (pge.ScreenWidth() - (2 * nebula->spr->width))), -(fNebulaeSpacer * ((float)nebula->spr->height) * nebula->scale) };
        }
    }
}