#pragma once

#include <engine/GameScreen.h>

enum ScreenId : Wind::GameScreenId {
	none = 255,
	mainMenu = 0,
	credits,
	settings,
	play,
	gameOver,
	gameComplete,
	leaveGame,
	levelComplete
};

