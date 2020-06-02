#pragma once

#include "FwdDecl.h"
#include "Math.h"

namespace Wind {

struct Sprite {
	/*	const char** bitmaps;
	    int          numBitmaps;
	    float        animationFreq;*/
	BitmapPtr bitmap;
	Vec2      position;
};

} // namespace Wind
