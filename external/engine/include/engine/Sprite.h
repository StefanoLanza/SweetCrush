#pragma once

#include "FwdDecl.h"
#include "Maths.h"

namespace Wind {

struct Sprite {
	/*	const char** bitmaps;
	    int          numBitmaps;
	    float        animationFreq;*/
	TexturePtr bitmap;
	Vec2      position;
};

} // namespace Wind
