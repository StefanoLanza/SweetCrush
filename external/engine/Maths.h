#pragma once

#include <cmath>
#include <limits>
#include <algorithm>

namespace Wind {

struct Vec2 {
	float x;
	float y;
};

struct Vec3 {
	float x;
	float y;
	float z;
};

struct Vec4 {
	float x;
	float y;
	float z;
	float w;
};

struct Rect {
	float left;
	float top;
	float right;
	float bottom;
};

struct RectI {
	int left;
	int top;
	int right;
	int bottom;
};

Vec2 operator+(const Vec2& lhs, const Vec2& rhs);
Vec2 operator-(const Vec2& lhs, const Vec2& rhs);
Vec2 operator*(const Vec2& v, float s);
Vec2 operator*(float s, const Vec2& v);
Vec2 operator-(const Vec2& v);
Vec2 Normalize(const Vec2& v);
bool operator==(const Vec2& lhs, const Vec2& rhs);
// TODO add more

bool RectContainsPoint(const Rect& rect, const Vec2& point);
Vec2 Lerp(Vec2 a, Vec2 b, float t);
Vec2 Spline(const Vec2 p[], int n, float t);
Vec2 Clamp(Vec2 v, Vec2 min, Vec2 max);

float Clamp(float x, float min, float max);
float Lerp(float a, float b, float t);

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

constexpr float pi = 3.14159265359f;
constexpr float two_pi = 6.28318530718f;

} // namespace Wind
