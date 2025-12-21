#pragma once

#include "Constants.h"
#include <cstdint>

using BoardDef = const char[NumRows * NumCols + 1];

struct LevelObjective {
	int gemCount[3];
};

struct Level {
	int             gemIds[MaxPieceTypes];
	LevelObjective  objective;
	uint32_t        seed;
	float           time; // seconds
	const BoardDef* boardDef;
};
