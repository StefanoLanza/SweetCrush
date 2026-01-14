#pragma once

enum class EffectType {
	hrocket,
	vrocket,
	miniBomb,
	bomb,
};
constexpr int NumEffects = static_cast<int>(EffectType::bomb) + 1;
