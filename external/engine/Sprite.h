#pragma once

#include "Math.h"
#include "FwdDecl.h"

namespace Wind {

struct Sprite {
	/*	const char** bitmaps;
	    int          numBitmaps;
	    float        animationFreq;*/
	BitmapPtr bitmap;
	Vec2      position;
};

} // namespace Wind
