#include "Levels.h"
#include "../src/Level.h"

#define TEST_SHORT_GAME 1

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
    "ccccdcbcb"
    "abcdabbdb"
    "bcdabcdab"
    "ccc   ccc"
    "ccc   ccc";

const Level levels[numLevels] {
	{
	    .pieceIds { 1, 2, 3, 4, 5 },
	    .goal { .id = GoalId::breakIce, .breakIce {} },
	    .seed = 24234,
	    .availableMoves = 24,
	    .boardDef = &testLevelGrid,
	    .boardMask = nullptr,
	},
};

#else

const Level levels[numLevels] {
	// #0
	{ .pieceIds { 0, 1, 3, 4, 5 },
	  .goal { GoalId::collectMatches, { 12, 12, 12 } },
	  .seed = 24234,
	  .availableMoves = 24,
	  //.boardMask = (const char*)crossMask,
	  .boosterIds { 0, 1, 2 },
	  .boosterCount { 2, 3, 4 } },
	// #1
	{ .pieceIds { 1, 2, 3, 5, 6 },
	  .goal { GoalId::collectMatches, { 15, 15, 15 } },
	  .seed = 224234,
	  .availableMoves = 24,
	  .boosterIds { 0, 1, 2 },
	  .boosterCount { 2, 3, 4 } },
	// #2
	{ .pieceIds { 0, 5, 3, 4, 7 },
	  .goal { GoalId::collectMatches, { 18, 18, 18 } },
	  .seed = 324234,
	  .availableMoves = 24,
	  .boosterIds { 0, 1, 2 },
	  .boosterCount { 2, 3, 4 } },
	// #3
	{ .pieceIds { 3, 4, 7, 5, 6 },
	  .goal { GoalId::collectMatches, { 21, 21, 21 } },
	  .seed = 324234,
	  .availableMoves = 24,
	  .boosterIds { 0, 1, 2 },
	  .boosterCount { 2, 3, 4 } },
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
