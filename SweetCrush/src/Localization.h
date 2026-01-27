#pragma once

#include <engine/StringTable.h>

struct GameStringId {
	enum Value {
		empty,
		title,
		score,
		start,
		play,
		settings,
		languageScreen,
		graphicsSettings,
		audioSettings,
		musicOn,
		musicOff,
		sfxOn,
		sfxOff,
		nextLanguage,
		credits,
		quit,
		gameOver,
		retry,
		toMainMenu,
		codeBy,
		graphicsBy,
		musicBy,
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
	};

	Value v;
    GameStringId(Value v) : v(v) {}

	// This allows: uint32_t id = GameID::Player;
    operator Wind::StringId() const { return static_cast<Wind::StringId>(v); }
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