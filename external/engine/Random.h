#pragma once

#include <cstdint>

namespace Wind {


class Random final {
public:

	Random();
	explicit Random(uint32_t seed);

	void Seed(uint32_t seed);
	int Next();
	int Next(int min, int max);
private:
	uint32_t NextState();
private:
	uint32_t mState;
};

}
