#ifndef BG_H
#define BG_H
#include "olcPixelGameEngine.h"
#include "Artifact.h"


#pragma once
class Background
{
public:
	Background(olc::PixelGameEngine&, float, int);
	void populateStart();
	void Update(float);
	void Draw();

	olc::PixelGameEngine& pge;
	float fWorldSpeed;
	int nFarStars;
	float fFarStarsSpeed;
	float fNebulaeSpacer;
	std::array<olc::vf2d, 800> arrStars;
	std::list<Artifact *> nebulas;
	
};

#endif // BG_H