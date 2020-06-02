#include "Levels.h"
#include "Level.h"

#if TEST_SHORT_GAME

const Level levels[numLevels] {
	{ { 1, 2, 3, 4, 5 }, 5, { 1, 0, 0 }, 24234, 60.f },
};

#else

const Level levels[numLevels] {
	{ { 1, 2, 3, 4, 5 }, 5, { 10, 10, 10 }, 24234, 60.f },
	{ { 0, 5, 3, 4, 6 }, 5, { 10, 10, 10 }, 224234, 60.f },
	{ { 0, 5, 3, 4, 7 }, 5, { 10, 10, 10 }, 324234, 60.f },
};

#endif
