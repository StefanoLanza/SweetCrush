#include "Random.h"

#include <algorithm>
#include <cassert>

namespace Wind {

Random::Random()
    : mState { 1 } {
}

Random::Random(uint32_t seed) {
	Seed(seed);
}

void Random::Seed(uint32_t seed) {
	if (seed == 0) {
		seed = 1; // avoid zero state
	}
	mState = seed;
}

int Random::Next() {
	return NextState();
}

int Random::Next(int min, int max) {
	if (min > max) {
		std::swap(min, max);
	}
	uint32_t r = Next() % static_cast<uint32_t>(max - min + 1);
	return min + static_cast<int>(r);
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
	return mState;
}

} // namespace Wind
