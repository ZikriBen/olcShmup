#ifndef ZOOMINGSTARSBG_H
#define ZOOMINGSTARSBG_H
#include "olcPixelGameEngine.h"
#include <random>

#pragma once
class ZoomingStarsBG
{
public:
	ZoomingStarsBG(olc::PixelGameEngine& pge) : pge(pge) {};
	void populateStars() {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int> disX(0, pge.ScreenWidth());
		std::uniform_int_distribution<int> disY(0, pge.ScreenHeight());

		for (int i = 0; i < numStars; ++i) {
			arrStars.push_back({ ((float)disX(gen)), ((float)disY(gen)) });
		};
	}
	
	void Update(float fElapsedTime) {
		for (auto& star : arrStars) {
			float moveX = (star.x < (pge.ScreenWidth() / 2)) ? -1.0f : 1.0f;
			float moveY = (star.y < (pge.ScreenHeight() / 2)) ? -1.0f : 1.0f;

			float dx = star.x - (pge.ScreenWidth() / 2);
			float dy = star.y - (pge.ScreenHeight() / 2);

			// Calculate normalized differences
			float normalized_dx = dx / (pge.ScreenWidth() / 2);
			float normalized_dy = dy / (pge.ScreenHeight() / 2);

			float distance = sqrt(dx * dx + dy * dy);
			float speedMultiplier = 1.0f + (distance / sqrt((pge.ScreenWidth() / 2.0f) * (pge.ScreenWidth() / 2.0f) + (pge.ScreenHeight() / 2.0f) * (pge.ScreenHeight() / 2.0f)));

			// Modify star movement based on proximity to midpoint
			star.x += normalized_dx * starSpeed * fElapsedTime * speedMultiplier;
			star.y += normalized_dy * starSpeed * fElapsedTime * speedMultiplier;

			if (star.x < 0 || star.x >= pge.ScreenWidth() || star.y < 0 || star.y >= pge.ScreenHeight()) {
				star.x = rand() % innerRectWidth + ((pge.ScreenWidth() / 2) - innerRectWidth / 2);
				star.y = rand() % innerRectHeight + ((pge.ScreenHeight() / 2) - innerRectHeight / 2);
			}
		}
	}

	void Draw() {
		for (auto& star : arrStars)
			pge.Draw(star, olc::WHITE);
	}

	olc::PixelGameEngine& pge;
	std::vector<olc::vf2d> arrStars;
	const int numStars = 300;
	const float starSpeed = 70.0f;
	const int innerRectWidth = 400;
	const int innerRectHeight = 300;
	
};

#endif //ZOOMINGSTARSBG_H
