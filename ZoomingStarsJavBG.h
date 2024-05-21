#ifndef ZOOMINGSTARSJAVBG_H
#define ZOOMINGSTARSJAVBG_H
#include "olcPixelGameEngine.h"
#include <random>

#pragma once
struct Star {
	float fAngle = 0.0f;
	float fDistance = 0.0f;
	float fSpeed = 0.0f;
	olc::Pixel col = olc::WHITE;
};

class ZoomingStarsJavBG
{
public:
	olc::PixelGameEngine& pge;
	std::vector<Star> vStars;
	olc::vf2d vOrigin;
	const int numStars = 1000;


	float Random(float a, float b) {
		return (b - a) * (float(rand()) / float(RAND_MAX)) + a;
	}

	ZoomingStarsJavBG(olc::PixelGameEngine& pge) : pge(pge) {};
	
	void populateStars() {
		vStars.resize(numStars);

		for (auto& star : vStars) {
			star.fAngle = Random(0.0f, 2.0f * 3.1459f);
			star.fSpeed = Random(10.0f, 100.0f);
			star.fDistance = Random(20.0f, 200.0f);
			float lum = Random(0.3f, 1.0f);
			star.col = olc::PixelF(lum, lum, lum, 1.0f);
		}
		
		vOrigin = { (float(pge.ScreenWidth() / 2)),(float(pge.ScreenHeight() / 2)) };
	}

	void Update(float fElapsedTime) {
		
		for (auto& star : vStars) {
			star.fDistance += star.fSpeed * fElapsedTime * (star.fDistance / 100.0f);
			if (star.fDistance > 500.0f) {
				star.fAngle = Random(0.0f, 2.0f * 3.1459f);
				star.fSpeed = Random(10.0f, 100.0f);
				star.fDistance = Random(1.0f, 100.0f);
				float lum = Random(0.3f, 1.0f);
				star.col = olc::PixelF(lum, lum, lum, 1.0f);
			}
		}
	}

	void Draw() {
		for (auto& star : vStars) {
			pge.Draw(olc::vf2d(cos(star.fAngle), sin(star.fAngle)) * star.fDistance + vOrigin, star.col * (star.fDistance / 100.0f));
		}
	}
};

#endif //ZOOMINGSTARSJAVBG_H
