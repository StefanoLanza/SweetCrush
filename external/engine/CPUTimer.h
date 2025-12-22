#pragma once

#include <chrono>

namespace Wind {

class CPUTimer final {
public:
	void Start();
	// Returns elapsed time in seconds as a double
	double End();
	void Reset() {}

private:
	using clock = std::chrono::steady_clock;
	clock::time_point mTimePoint;
};

} // namespace Wind
