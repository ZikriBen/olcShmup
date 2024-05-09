#include "Artifact.h"


Artifact::Artifact(olc::vf2d pos, olc::Sprite* spr) : pos(pos), spr(spr), scale(1), flip(0) {}

Artifact::Artifact(olc::vf2d pos, olc::Sprite* spr, int scale) : pos(pos), spr(spr), scale(scale), flip(0) {}

Artifact::Artifact(olc::vf2d pos, olc::Sprite* spr, int scale, int flip) : pos(pos), spr(spr), scale(scale), flip(flip) {}
