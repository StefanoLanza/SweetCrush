#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>

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

struct Int2 {
	int x;
	int y;
};

Vec2 operator+(const Vec2& lhs, const Vec2& rhs);
Vec2 operator+(const Vec2& v, float s);
Vec2 operator-(const Vec2& lhs, const Vec2& rhs);
Vec2 operator*(const Vec2& lhs, const Vec2& rhs);
Vec2 operator*(const Vec2& v, float s);
Vec2 operator*(float s, const Vec2& v);
Vec2 operator/(const Vec2& lhs, const Vec2& rhs);
Vec2 operator/(const Vec2& v, float s);
Vec2 operator/(float s, const Vec2& v);
Vec2 Normalize(const Vec2& v);
Vec2 operator-(const Vec2& v);
bool operator==(const Vec2& lhs, const Vec2& rhs);
// TODO add more

bool RectContainsPoint(const Rect& rect, const Vec2& point);
Vec2 Lerp(Vec2 a, Vec2 b, float t);
Vec2 Spline(const Vec2 p[], int n, float t);
Vec2 Clamp(Vec2 v, Vec2 min, Vec2 max);
Vec2 Ortho(Vec2 v);

template <typename Curve>
Vec2 LerpEase(Vec2 a, Vec2 b, float t, Curve&& curve) {
	t = curve(t);
	return Lerp(a, b, t);
}

float Clamp(float x, float min, float max);

inline float Lerp(float a, float b, float t) {
	return a + (b - a) * t;
}

template <typename Curve>
float LerpEase(float a, float b, float t, Curve&& curve) {
	return a + (b - a) * curve(t);
}

inline float Interpolate(float p0, float p1, float p2, float t) {
	// Simplified Lagrange coefficients
	float a = 2.0f * p0 - 4.0f * p1 + 2.0f * p2;
	float b = -3.0f * p0 + 4.0f * p1 - p2;
	float c = p0;
	return (a * t * t) + (b * t) + c;
}

constexpr float pi = 3.14159265359f;
constexpr float half_pi = pi * 0.5f;
constexpr float two_pi = pi * 2.f;

} // namespace Wind
