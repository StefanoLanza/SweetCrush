#pragma once

#include <engine/Screen.h>

enum class GameScreenIds : Wind::ScreenId::Type {
	mainMenu,
	credits,
	settings,
	play,
	gameOver,
	gameComplete,
	pauseGame,
	levelComplete,
	graphicsSettings,
	audioSettings,
	effectInfo,
	language,
	levelStart,
};
