#pragma once

#include <engine/StringTable.h>

enum GameStringId {
	empty,
	title,
	tap,
	time_,
	moves,
	score,
	start,
	play,
	settings,
	languageScreen,
	graphicsSettings,
	audioSettings,
	music,
	sfx,
	nextLanguage,
	credits,
	quit,
	gameOver,
	retry,
	toMainMenu,
	codeBy,
	graphicsBy,
	musicBy,
	fontBy,
	version,
	back,
	level,
	goal,
	yourScoreIs,
	yourFinalScoreIs,
	yourReachedLevel,
	youCompletedAllLevels,
	pauseGame,
	continueGame,
	restartLevel,
	endGame,
	ok,
	complete,
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
};

enum class Language {
	english,
	spanish,
	italian
};
constexpr int NumLanguages = 3;

void        SetLanguage(Language language);
void        SetNextLanguage();
Language    GetCurrentLanguage();
const char* GetLocalizedString(GameStringId str);