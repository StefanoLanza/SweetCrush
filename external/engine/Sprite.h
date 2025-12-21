#pragma once

#include "FwdDecl.h"
#include "Maths.h"

namespace Wind {

struct Sprite {
	/*	const char** bitmaps;
	    int          numBitmaps;
	    float        animationFreq;*/
	BitmapPtr bitmap;
	Vec2      position;
};

} // namespace Wind
