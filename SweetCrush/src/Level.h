#pragma once

#include "Constants.h"
#include <cstdint>

using BoardDef = const char[NumRows * NumCols + 1];

enum class GoalId {
	collectMatches,
	removeJellies,
	breakIce,
	collectAllStars,
};

struct GoalCollectMatches {
	int count[3]; // values <= 0 are ignored
};

struct GoalCollectAllStars {
	int starCount;
};

struct GoalBreakIce {
	int iceBlocksCount; // if procedurally generated
};

struct Goal {
	GoalId id;
	union {
		GoalCollectMatches  collectMatches;
		GoalCollectAllStars collectAllStars;
		GoalBreakIce        breakIce;
	};
};

struct Level {
	int             pieceIds[MaxPieceTypes];
	Goal            goal;
	uint32_t        seed;
	float           availTime; // seconds
	const BoardDef* boardDef;  // null for random boards
	const char*     boardMask; // can be null
};
