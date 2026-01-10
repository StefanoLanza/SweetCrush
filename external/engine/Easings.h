#pragma once

#include "Maths.h"

namespace Wind {

// ref: https://easings.net

inline float EaseInSine(float x) {
	return 1 - std::cos(x * pi) / 2.f;
}

inline float EaseInQuad(float x) {
	return x * x;
}

inline float EaseInCubic(float x) {
	return x * x * x;
}

inline float EaseInQuint(float x) {
	return x * x * x * x * x;
}

inline float EaseInElastic(float x) {
	constexpr float c4 = two_pi / 3.f;
	return x == 0.f ? 0.f : x == 1.f ? 1.f : -std::pow(2.f, 10 * x - 10) * std::sin((x * 10 - 10.75f) * c4);
}

inline float EaseInCirc(float x) {
	return 1 - std::sqrt(1 - std::pow(x, 2.f));
}

inline float EaseInOutCirc(float x) {
	return x < 0.5 ? (1 - std::sqrt(1 - std::pow(2 * x, 2.f))) / 2 : (std::sqrt(1 - std::pow(-2 * x + 2, 2.f)) + 1) / 2;
}

inline float EaseOutBounce(float x) {
	constexpr float n1 = 7.5625f;
	constexpr float d1 = 2.75f;
	if (x < 1 / d1) {
		return n1 * x * x;
	}
	else if (x < 2.f / d1) {
		return n1 * (x -= 1.5f / d1) * x + 0.75f;
	}
	else if (x < 2.5f / d1) {
		return n1 * (x -= 2.25f / d1) * x + 0.9375f;
	}
	else {
		return n1 * (x -= 2.625f / d1) * x + 0.984375f;
	}
}

} // namespace Wind
