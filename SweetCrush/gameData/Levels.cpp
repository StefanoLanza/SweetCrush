#include "Levels.h"
#include "../src/Level.h"

#define TEST_SHORT_GAME 0

namespace {

// clang-format off
const char heartMask[NumRows][NumCols] = {
    {' ', ' ', 'X', 'X', ' ', 'X', 'X', ' ', ' '},
    {' ', 'X', 'X', 'X', 'X', 'X', 'X', 'X', ' '},
    {'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X'},
    {'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X'},
    {'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X'},
    {' ', 'X', 'X', 'X', 'X', 'X', 'X', 'X', ' '},
    {' ', ' ', 'X', 'X', 'X', 'X', 'X', ' ', ' '},
    {' ', ' ', ' ', 'X', 'X', 'X', ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', 'X', ' ', ' ', ' ', ' '}
};

const char diamondMask[9][9] = {
    {' ', ' ', ' ', ' ', 'X', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', 'X', 'X', 'X', ' ', ' ', ' '},
    {' ', ' ', 'X', 'X', 'X', 'X', 'X', ' ', ' '},
    {' ', 'X', 'X', 'X', 'X', 'X', 'X', 'X', ' '},
    {'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X'},
    {' ', 'X', 'X', 'X', 'X', 'X', 'X', 'X', ' '},
    {' ', ' ', 'X', 'X', 'X', 'X', 'X', ' ', ' '},
    {' ', ' ', ' ', 'X', 'X', 'X', ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', 'X', ' ', ' ', ' ', ' '}
};

const char starMask[9][9] = {
    {' ', ' ', ' ', ' ', 'X', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', 'X', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', 'X', 'X', 'X', ' ', ' ', ' '},
    {'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X'},
    {' ', 'X', 'X', 'X', 'X', 'X', 'X', 'X', ' '},
    {' ', ' ', 'X', 'X', 'X', 'X', 'X', ' ', ' '},
    {' ', ' ', 'X', 'X', ' ', 'X', 'X', ' ', ' '},
    {' ', 'X', 'X', ' ', ' ', ' ', 'X', 'X', ' '},
    {'X', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'X'}
};

const char crossMask[9][9] = {
    {' ', ' ', ' ', 'X', 'X', 'X', ' ', ' ', ' '},
    {' ', ' ', ' ', 'X', 'X', 'X', ' ', ' ', ' '},
    {' ', ' ', ' ', 'X', 'X', 'X', ' ', ' ', ' '},
    {'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X'},
    {'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X'},
    {'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X'},
    {' ', ' ', ' ', 'X', 'X', 'X', ' ', ' ', ' '},
    {' ', ' ', ' ', 'X', 'X', 'X', ' ', ' ', ' '},
    {' ', ' ', ' ', 'X', 'X', 'X', ' ', ' ', ' '}
};

// clang-format on

#if TEST_SHORT_GAME
constexpr int numLevels = 1;
#else
constexpr int numLevels = 4;
#endif

#if TEST_SHORT_GAME

const BoardDef testLevelGrid = //
    "ccccscacb"
    "cCccdcCcb"
    "cccc cccb"
    "  cabaccb"
    "ccccOcbcb"
    "abcdabbdb"
    "bcdabcdab"
    "ccc   ccc"
    "ccc   ccc";

const Level levels[numLevels] {
	{ { 1, 2, 3, 4, 5 }, { GoalId::breakAllIce }, 24234, 600.f, &testLevelGrid },
};

#else

const Level levels[numLevels] {
	{ { 0, 2, 3, 4, 5 }, { GoalId::collectMatches, { 12, 12, 12 } }, 24234, 60.f, nullptr, (const char*)crossMask },
	{ { 1, 2, 3, 5, 6 }, { GoalId::collectMatches, { 15, 15, 15 } }, 224234, 60.f, nullptr },
	{ { 0, 5, 3, 4, 7 }, { GoalId::collectMatches, { 18, 18, 18 } }, 324234, 60.f, nullptr },
	{ { 3, 4, 8, 5, 6 }, { GoalId::collectMatches, { 21, 21, 21 } }, 324234, 60.f, nullptr },
};

#endif

} // namespace

extern "C"
{

	int GetNumLevels() {
		return numLevels;
	}

	const Level* GetLevel(int levelIndex) {
		return &levels[levelIndex];
	}
}
