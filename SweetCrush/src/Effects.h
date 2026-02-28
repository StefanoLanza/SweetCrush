#pragma once

enum class EffectType {
	none,
	hStriped,
	vStriped,
	wrapped,
	colorBomb,
};
constexpr int NumEffects = static_cast<int>(EffectType::colorBomb) + 1;
