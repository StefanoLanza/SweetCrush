#pragma once

namespace Wind {

// Channels in the 0, 255 range
struct Color {
	float r;
	float g;
	float b;
	float a;
};

constexpr Color blackColor = { 0.f, 0.f, 0.f, 255.f };
constexpr Color whiteColor = { 255.f, 255.f, 255.f, 255.f };
constexpr Color redColor = { 255.f, 0.f, 0.f, 255.f };
constexpr Color greenColor = { 0.f, 255.f, 0.f, 255.f };
constexpr Color blueColor = { 0.f, 0.f, 255.f, 255.f };
constexpr Color yellowColor = { 255.f, 255.f, 0.f, 255.f };
constexpr Color darkGreenColor = { 34.f, 87.f, 0.f, 255.f };
constexpr Color purpleColor = { 240.f, 91.f, 241.f, 255.f };
constexpr Color lightBlueColor = { 56.f, 105.f, 255.f, 255.f };

} // namespace Wind
