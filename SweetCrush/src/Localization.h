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
	replay,
	toMainMenu,
	codeBy,
	graphicsBy,
	audioBy,
	back,
	yourScoreIs,
	yourFinalScoreIs,
	yourReachedLevel,
	youCompletedAllLevels,
	leave,
	yes,
	no,
	ok,
	levelComplete,
	nextLevel,
	gameComplete,
	exitGame,
	// boosters
	miniBomb,
	miniBombDescription,
	bomb,
	bombDescription,
	star,
	starDescription,
	//
};

enum class Language {
	english,
	spanish,
	italian
};
constexpr int NumLanguages = 3;

void SetLanguage(Language language);
void SetNextLanguage();
Language GetCurrentLanguage();
const char* GetLocalizedString(GameStringId str);