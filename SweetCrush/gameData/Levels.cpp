#include "Levels.h"
#include "../src/Level.h"

#define TEST_SHORT_GAME 1

namespace {

#if TEST_SHORT_GAME
constexpr int numLevels = 1;
#else
constexpr int numLevels = 4;
#endif

#if TEST_SHORT_GAME

const BoardDef testLevelGrid =
	"ccccdccc"
	"cCccdcCc"
	"ccccHccc"
	"EEcabacc"
	"ccccaccc"
	"abcdabcd"
	"bcdabcda"
	"cccHHccc";

const Level levels[numLevels] {
	{ { 1, 2, 3, 4, 5 }, { 1, 0, 0 }, 24234, 60.f, &testLevelGrid },
};

#else

const Level levels[numLevels] {
	{ { 0, 2, 3, 4, 5 }, { 12, 12, 12 }, 24234, 60.f, nullptr },
	{ { 1, 2, 3, 5, 6 }, { 15, 15, 15 }, 224234, 60.f, nullptr },
	{ { 0, 5, 3, 4, 7 }, { 18, 18, 18 }, 324234, 60.f, nullptr },
	{ { 3, 4, 8, 5, 6 }, { 21, 21, 21 }, 324234, 60.f, nullptr },
};

#endif

} // namespace

extern "C" {

int GetNumLevels() {
	return numLevels;
}

const Level* GetLevel(int levelIndex) {
	return &levels[levelIndex];
}

}
