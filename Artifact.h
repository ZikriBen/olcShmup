#ifndef ARTIFACT_H
#define ARTIFACT_H
#include "olcPixelGameEngine.h"

#pragma once
class Artifact
{
public:
	Artifact();
	Artifact(olc::vf2d, olc::Sprite*);
	Artifact(olc::vf2d, olc::Sprite*, int scale);
	Artifact(olc::vf2d, olc::Sprite*, int scale, int flip);
	olc::vf2d pos;
	olc::Sprite* spr;
	int scale;
	int flip;
};

#endif // ARTIFACT_H

