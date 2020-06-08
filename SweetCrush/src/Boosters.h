#pragma once

enum class BoosterType {
	hrocket,
	vrocket,
	miniBomb,
	bomb,
};
constexpr int NumBoosters = static_cast<int>(BoosterType::bomb) + 1;
