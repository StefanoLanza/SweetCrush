#include "Levels.h"
#include <src/Level.h>

#define TEST_SHORT_GAME 0

namespace {

#if TEST_SHORT_GAME
constexpr int numLevels = 1;
#else
constexpr int numLevels = 4;
#endif

#if TEST_SHORT_GAME

const Level levels[numLevels] {
	{ { 1, 2, 3, 4, 5 }, 5, { 1, 0, 0 }, 24234, 60.f },
};

#else

const Level levels[numLevels] {
	{ { 0, 2, 3, 4, 5 }, 5, { 12, 12, 12 }, 24234, 60.f },
	{ { 1, 2, 3, 5, 6 }, 5, { 15, 15, 15 }, 224234, 60.f },
	{ { 0, 5, 3, 4, 7 }, 5, { 18, 18, 18 }, 324234, 60.f },
	{ { 3, 4, 8, 5, 6 }, 5, { 21, 21, 21 }, 324234, 60.f },
};

#endif

} // namespace

extern "C" {

int GetNumLevels() {
	return numLevels;
}

const Level& GetLevel(int levelIndex) {
	return levels[levelIndex];
}

}
