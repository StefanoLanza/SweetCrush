#pragma once

#include "Maths.h"
#include <cassert>

namespace Wind {

// Channels in the 0, 255 range
struct Color {
	float r;
	float g;
	float b;
	float a;

	explicit inline operator Vec4() const {
		return { r / 255.f, g / 255.f, b / 255.f, a / 255.f };
	}
};

constexpr Color transparentColor = { 0.f, 0.f, 0.f, 0.f };
constexpr Color blackColor = { 0.f, 0.f, 0.f, 255.f };
constexpr Color whiteColor = { 255.f, 255.f, 255.f, 255.f };
constexpr Color redColor = { 255.f, 0.f, 0.f, 255.f };
constexpr Color greenColor = { 0.f, 255.f, 0.f, 255.f };
constexpr Color blueColor = { 0.f, 0.f, 255.f, 255.f };
constexpr Color yellowColor = { 255.f, 255.f, 0.f, 255.f };
constexpr Color darkGreenColor = { 34.f, 87.f, 0.f, 255.f };
constexpr Color purpleColor = { 240.f, 91.f, 241.f, 255.f };
constexpr Color lightBlueColor = { 56.f, 105.f, 255.f, 255.f };

inline Color Lerp(const Color& a, const Color& b, float t) {
	assert(t >= 0.f && t <= 1.f);
	return {
		a.r + (b.r - a.r) * t,
		a.g + (b.g - a.g) * t,
		a.b + (b.b - a.b) * t,
		a.a + (b.a - a.a) * t,
	};
}

inline Color Mul(const Color& first, const Color& second) {
	return {
		(first.r * second.r / 255.f),
		(first.g * second.g / 255.f),
		(first.b * second.b / 255.f),
		(first.a * second.a / 255.f),
	};
}

inline Color Interpolate(const Color& first, const Color& second, const Color& third, float t) {
	return {
		Interpolate(first.r, second.r, third.r, t),
		Interpolate(first.g, second.g, third.g, t),
		Interpolate(first.b, second.b, third.b, t),
		Interpolate(first.a, second.a, third.a, t),
	};
}

inline Color operator*(const Color& color, float s) {
	return {
		color.r * s,
		color.g * s,
		color.b * s,
		color.a * s,
	};
}

} // namespace Wind
