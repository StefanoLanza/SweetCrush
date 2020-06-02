#pragma once

#include <cstdint>

struct Level {
	int      gemIds[5];
	int      numGemIds;
	int      targetGemCount[3];
	uint32_t seed;
	float    time; // seconds
};
