#pragma once

#include "Constants.h"
#include <cstdint>

struct UserGrid {
	unsigned char tiles[NumRows][NumCols];
};

struct LevelObjective {
	int gemCount[3];
};

struct Level {
	int            gemIds[5];
	int            numGemIds;
	LevelObjective objective;
	uint32_t       seed;
	float          time; // seconds
	                     // const UserGrid* grid;
};
