#pragma once

#include "Constants.h"
#include <cstdint>

using BoardDef = const char[NumRows * NumCols + 1];

enum class GoalId {
	collectMatches,
	removeJellies,
	breakAllIce,
};

struct GoalCollectMatches {
	int count[3]; // three count
};

struct Goal {
	GoalId id;
	union {
		GoalCollectMatches collectMatches;
	};
};

struct Level {
	int             gemIds[MaxPieceTypes];
	Goal            goal;
	uint32_t        seed;
	float           time; // seconds
	const BoardDef* boardDef;
};
