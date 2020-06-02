#pragma once

namespace Wind {

struct Vec2 {
	float x;
	float y;
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
} // namespace Wind
