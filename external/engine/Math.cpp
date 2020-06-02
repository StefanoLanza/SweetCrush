#include "Math.h"

#include <cassert>
#include <cmath>
#include <algorithm>

namespace Wind {
	Vec2 operator+(const Vec2& lhs, const Vec2& rhs) {
		return { lhs.x + rhs.x, lhs.y + rhs.y };
	}

	Vec2 operator-(const Vec2& lhs, const Vec2& rhs) {
		return { lhs.x - rhs.x, lhs.y - rhs.y };
	}

	Vec2 operator*(const Vec2& v, float s) {
		return { v.x * s, v.y * s };
	}

	Vec2 operator*(float s, const Vec2& v) {
		return v * s;
	}

	Vec2 operator-(const Vec2& v) {
		return { -v.x, -v.y };
	}

	bool operator==(const Vec2& lhs, const Vec2& rhs) {
		constexpr float eps = 1e-6f;
		return (std::abs(lhs.x - rhs.x) < eps) && (std::abs(lhs.y - rhs.y) < eps);
	}

	Vec2 Normalize(const Vec2& v) {
		const float l2 = v.x * v.x + v.y * v.y;
		constexpr float eps = 1e-6f;
		if (l2 < eps) {
			return { 0.f, 0.f };
		}
		const float l = std::sqrt(l2);
		return { v.x / l, v.y / l };
	}

	float Clamp(float x, float min, float max) {
		if (min > max) {
			std::swap(min, max);
		}
		return std::clamp(x, min, max);
	}

	Vec2 Clamp(Vec2 v, Vec2 min, Vec2 max) {
		return { Clamp(v.x, min.x, max.x), Clamp(v.y, min.y, max.y) };
	}


	bool RectContainsPoint(const Rect& rect, const Vec2& point) {
		return std::clamp(point.x, rect.left, rect.right) == point.x &&
			std::clamp(point.y, rect.top, rect.bottom) == point.y;
	}

	Vec2 Lerp(Vec2 a, Vec2 b, float t) {
		return { Lerp(a.x, b.x, t), Lerp(a.y, b.y, t) };
	}

	Vec2 Spline(const Vec2 p[], int n, float t) {
		assert(n >= 4);
		t = std::clamp(t, 0.f, 1.f);

		float curve_p = t * (n - 1);
		int curve_num = static_cast<int>(std::floor(curve_p));
		t = curve_p - curve_num;
	
		int b = curve_num;
		int a = std::max(0, b - 1);
		int c = b + 1;
		int d = std::min(c + 1, n - 1);
		const Vec2& p0 = p[a];
		const Vec2& p1 = p[b];
		const Vec2& p2 = p[c];
		const Vec2& p3 = p[d];

		float t2 = t*t;
		float t3 = t*t*t;
		return  0.5f * ((2 * p1) + (-p0 + p2) * t + (2 * p0 - 5 * p1 + 4 * p2 - p3) * t2 + (-p0 + 3 * p1 - 3 * p2 + p3) * t3);
	}

	float Lerp(float a, float b, float t) {
		return a + (b - a) * t;
	}
}