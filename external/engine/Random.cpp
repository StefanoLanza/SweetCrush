#include "Random.h"

#include <algorithm>
#include <cassert>

namespace Wind {

Random::Random()
    : mState { 1 } {
	Seed(1);
}

Random::Random(uint32_t seed) {
	Seed(seed);
}

void Random::Seed(uint32_t seed) {
// A simple 32-bit "SplitMix" style hash to scramble the seed
    seed = (seed ^ (seed >> 16)) * 0x45d9f3b;
    seed = (seed ^ (seed >> 16)) * 0x45d9f3b;
    seed = seed ^ (seed >> 16);
    
    mState = (seed == 0) ? 42 : seed; // ensure not zero
}

int Random::Next() {
	return NextState();
}

int Random::Next(int min, int max) {
	if (min > max) {
		std::swap(min, max);
	}
	uint32_t span = Next() % static_cast<uint32_t>(max - min + 1);
	return min + static_cast<int>(span);
}

float Random::NextF() {
	double t = static_cast<double>(NextState()) * (1.0 / 4294967296.0);
	return static_cast<float>(t);
}

float Random::NextF(float min, float max) {
	if (min > max) {
		std::swap(min, max);
	}
	double t = static_cast<double>(NextState()) * (1.0 / 4294967296.0);
	return static_cast<float>(min + (max - min) * t);
}

uint32_t Random::NextState() {
	// Core xorshift RNG (deterministic everywhere)
	mState ^= mState << 13;
	mState ^= mState >> 17;
	mState ^= mState << 5;
	// Scramble the output with a 32-bit multiplier constant
    // 0x2545F491 is a common choice for 32-bit scrambling
    return mState * 0x2545F491;
}

} // namespace Wind
