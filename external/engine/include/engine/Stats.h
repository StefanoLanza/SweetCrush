#pragma once

#include <algorithm>
#include <cmath>
#include <limits>

namespace Wind {

class Stats {
public:
	void AddSample(double v) {
		++mSampleCount;
		double delta = (v - mMean);
		mMean += delta / mSampleCount;
		double delta2 = v - mMean;
		mM2 += delta * delta2;
		mMin = std::min(mMin, v);
		mMax = std::max(mMax, v);
	}
	void Reset() {
		mMean = 0.0;
		mMin = std::numeric_limits<double>::max();
		mMax = std::numeric_limits<double>::min();
		mM2 = 0.0;
		mSampleCount = 0;
	}
	double Mean() const {
		return mMean;
	}
	double Min() const {
		return mMin;
	}
	double Max() const {
		return mMax;
	}
	double SampleVariance() const {
		return (mSampleCount > 1) ? (mM2 / (mSampleCount - 1)) : 0.0;
	}
	double SampleStdDev() const {
		return std::sqrt(SampleVariance());
	}
	double PopulationVariance() const {
		return mSampleCount ? (mM2 / mSampleCount) : 0.0;
	}
	double PopulationStdDev() const {
		return std::sqrt(PopulationVariance());
	}
	int SampleCount() const {
		return mSampleCount;
	}

private:
	double mMean = 0.0;
	double mMin = std::numeric_limits<double>::max();
	double mMax = std::numeric_limits<double>::min();
	double mM2 = 0.0;
	int    mSampleCount = 0;
};

} // namespace Wind
