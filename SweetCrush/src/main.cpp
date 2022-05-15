// ! Include this before SDL_main
#ifdef _WIN32
#define NOMINMAX
struct IUnknown;
#include <Windows.h>
#endif

#include <SDL2/SDL_main.h>
#include <SDL2/SDL.h>
#include <engine/Engine.h>
#include <inih/ini.h>

#include "Config.h"
#include "Game.h"
#include "GameDataModule.h"
#include <algorithm>
#include <cstring>

namespace {
void LoadGameConfig(GameConfig& gameConfig, const char* iniFile);
}

int main(int argc, char* argv[]) {
	(void)argc;
	(void)argv;

#ifdef _WIN32
	ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif
	SDL_LogInfo(0, "Loading game config");
	GameConfig gameConfig = DefaultGameConfig();
	LoadGameConfig(gameConfig, ASSETS_FOLDER "game.ini");

	GameDataModule gameDataModule;
#ifndef __ANDROID__
	#ifdef _WIN32
	const char* dllName = "gameData.dll";
	#else
	const char* dllName = "./libgameData.so";
	#endif
	SDL_LogInfo(0, "Initializing game data module");
	if (!gameDataModule.Init(dllName)) {
		return 0;
	}
#endif
	Wind::Engine engine { "SweetCrush", gameConfig.windowWidth, gameConfig.windowHeight };

	Game game { engine, gameConfig, gameDataModule };
	game.Run();

	return 0;
}

namespace {

#define PARSE_INT(varName, minv, maxv)                             \
	if (! strcmp(name, #varName)) {                                \
		config.varName = std::clamp(std::atoi(value), minv, maxv); \
	}

#define PARSE_FLOAT(varName, minv, maxv)                                      \
	do {                                                                      \
		if (! strcmp(name, #varName)) {                                       \
			config.varName = std::clamp((float)std::atof(value), minv, maxv); \
		}                                                                     \
	} while (0)

#define PARSE_BOOL(varName)                                    \
	if (! strcmp(name, #varName)) {                            \
		config.varName = strcmp(value, "true") ? false : true; \
	}

int INIParser(void* user, const char* /*section*/, const char* name, const char* value) {
	GameConfig& config = *static_cast<GameConfig*>(user);
	PARSE_INT(windowWidth, 0, 3456);
	PARSE_INT(windowHeight, 0, 2234);
	PARSE_FLOAT(cellHeight, 16.f, 64.f);
	PARSE_FLOAT(cellWidth, 16.f, 64.f);
	PARSE_FLOAT(cellSpacing, 0.f, 8.f);
	// Animations
	PARSE_FLOAT(tileMoveBackSpeed, 1.f, 1024.f);
	PARSE_FLOAT(tileSwapSpeed, 1.f, 1024.f);
	PARSE_FLOAT(tileFallSpeed, 1.f, 1024.f);
	PARSE_FLOAT(tileFallYCoord, -1000.f, 0.f);
	// UI
	PARSE_FLOAT(swapThreshold, 0.1f, 1.f);
	PARSE_FLOAT(startDragThreshold, 1.f, 8.f);
	PARSE_BOOL(audioOn);

	config.cellWidthWithSpacing = config.cellWidth + config.cellSpacing;
	config.cellHeightWithSpacing = config.cellHeight + config.cellSpacing;
	return 1;
}

void LoadGameConfig(GameConfig& gameConfig, const char* iniFile) {
	ini_parse(iniFile, INIParser, &gameConfig);
	// Validation
}
} // namespace
