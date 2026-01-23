#pragma once

#include <engine/GameScreen.h>

enum ScreenId : Wind::GameScreenId {
	empty = 255,
	mainMenu = 0,
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
};
