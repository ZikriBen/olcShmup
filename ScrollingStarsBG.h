#ifndef SCROLLINGSTARSBG_H
#define SCROLLINGSTARSBG_H
#include "olcPixelGameEngine.h"
#include "Artifact.h"

constexpr auto NUM_STARS = 800;
#pragma once
class ScrollingStarsBG
{
public:
	ScrollingStarsBG(olc::PixelGameEngine&, float, int);
	void populateStars();
	void Update(float);
	void Draw();

	olc::PixelGameEngine& pge;
	float fWorldSpeed;
	int nFarStars;
	float fFarStarsSpeed;
	float fNebulaeSpacer;
	std::array<olc::vf2d, NUM_STARS> arrStars;
	std::list<Artifact *> nebulas;
	
};

#endif // SCROLLINGSTARSBG_H