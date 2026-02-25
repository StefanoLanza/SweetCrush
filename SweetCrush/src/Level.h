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

// Collect a target number of matches for 3 piece types
struct GoalCollectMatches {
	int count[MaxMatchesPerLevel]; // values <= 0 are ignored
};

struct GoalCollectAllStars {
	int starCount;
};

struct GoalBreakIce {
	int iceBlocksCount; // if procedurally generated
};

// Remove all pieces
struct GoalRemoveAllPieces {};

struct Goal {
	GoalId id;
	union {
		GoalCollectMatches  collectMatches;
		GoalCollectAllStars collectAllStars;
		GoalBreakIce        breakIce;
		GoalRemoveAllPieces removeAllPieces;
	};
};

struct Level {
	int             pieceIds[MaxPieceTypesPerLevel];
	Goal            goal;
	uint32_t        seed = 1224;
	int             availableMoves = 0;
	const BoardDef* boardDef = nullptr;  // null for random boards
	const char*     boardMask = nullptr; // can be null
	uint8_t         boosterIds[MaxBoosterTypesPerLevel];
	uint8_t         boosterCount[MaxBoosterTypesPerLevel];
};
