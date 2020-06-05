#pragma once

#include "Level.h"

#define TEST_SHORT_GAME 0

#if TEST_SHORT_GAME
constexpr int numLevels = 1;
#else
constexpr int numLevels = 4;
#endif
extern const Level levels[numLevels];
