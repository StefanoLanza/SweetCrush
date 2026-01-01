#include "CPUTimer.h"

namespace Wind {

void CPUTimer::Start() {
	mTimePoint = clock::now();
}

double CPUTimer::End() {
	std::chrono::duration<double> diff = clock::now() - mTimePoint;
	return diff.count();
}

} // namespace Wind
