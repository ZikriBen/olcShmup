#ifndef ARTIFACT_H
#define ARTIFACT_H
#include "olcPixelGameEngine.h"

#pragma once
class Artifact
{
public:
	Artifact() : spr(nullptr), scale(1), flip(0) {};
	Artifact(olc::vf2d pos, olc::Sprite* spr) : pos(pos), spr(spr), scale(1), flip(0) {};
	Artifact(olc::vf2d pos, olc::Sprite* spr, int scale) : pos(pos), spr(spr), scale(scale), flip(0) {};
	Artifact(olc::vf2d pos, olc::Sprite* spr, int scale, int flip) : pos(pos), spr(spr), scale(scale), flip(flip) {};
	olc::vf2d pos;
	olc::Sprite* spr;
	int scale;
	int flip;
};

#endif // ARTIFACT_H

