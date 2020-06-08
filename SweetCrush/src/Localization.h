#pragma once

#include <engine/StringTable.h>

enum class GameStringId : Wind::StringId {
	empty,
	title,
	score,
	start,
	settings,
	audioOn,
	audioOff,
	nextLanguage,
	credits,
	quit,
	gameOver,
	retry,
	toMainMenu,
	codeBy,
	graphicsBy,
	audioBy,
	back,
	yourScoreIs,
	yourFinalScoreIs,
	yourReachedLevel,
	youCompletedAllLevels,
	pauseGame,
	continueGame,
	restartLevel,
	exitGame,
	ok,
	levelComplete,
	nextLevel,
	gameComplete,
	// boosters
	hRocket,
	hRocketDescription,
	vRocket,
	vRocketDescription,
	miniBomb,
	miniBombDescription,
	bomb,
	bombDescription,
	//
};

enum class Language { english, spanish, italian };
constexpr int NumLanguages = 3;

void        SetLanguage(Language language);
void        SetNextLanguage();
Language    GetCurrentLanguage();
const char* GetLocalizedString(GameStringId str);