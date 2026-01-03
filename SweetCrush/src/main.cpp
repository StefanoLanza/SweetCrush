#include <SDL3/SDL_main.h>
#include <engine/Engine.h>
#include <engine/Sdl.h>
#include <engine/SdlWindow.h>

#include "Game.h"
#include "GameConfig.h"
#include "GameDataModule.h"
#include "IniParser.h"

#include <algorithm>
#include <cstring>

namespace {

void LoadGameConfig(GameConfig& gameConfig, const char* iniFile);
}

int main(int argc, char* argv[]) {
	GameConfig gameConfig = DefaultGameConfig();
	LoadGameConfig(gameConfig, ASSETS_FOLDER "game.ini");

	GameDataModule gameDataModule;
#ifndef __ANDROID__
#ifdef _WIN32
	const char* dllName = "gameData.dll";
#else
	const char* dllName = "./libgameData.so";
#endif
	if (! gameDataModule.Init(dllName)) {
		return 0;
	}
#endif
	Wind::Sdl       sdl { SDL_INIT_VIDEO | SDL_INIT_EVENTS };
	Wind::SdlWindow window { "SweetCrush", gameConfig.windowWidth, gameConfig.windowHeight, ASSETS_FOLDER "icon.png", gameConfig.fullscreen };
	Wind::Engine    engine { window };
	Game            game { engine, gameConfig, gameDataModule };
	game.Run();

	return 0;
}

namespace {

int INIParser(void* user, const char* /*section*/, const char* name, const char* value) {
	auto config = static_cast<GameConfig*>(user);
	PARSE_INT(config->windowWidth, "windowWidth", 0, 3456);
	PARSE_INT(config->windowHeight, "windowHeight", 0, 2234);
	PARSE_BOOL(config->fullscreen, "fullscreen");
	PARSE_FLOAT(config->cellHeight, "cellHeight", 16.f, 64.f);
	PARSE_FLOAT(config->cellWidth, "cellWidth", 16.f, 64.f);
	PARSE_FLOAT(config->cellSpacing, "cellSpacing", 0.f, 8.f);
	// Animations
	PARSE_FLOAT(config->tileMoveBackSpeed, "tileMoveBackSpeed", 1.f, 1024.f);
	PARSE_FLOAT(config->tileSwapSpeed, "tileSwapSpeed", 1.f, 1024.f);
	PARSE_FLOAT(config->tileFallSpeed, "tileFallSpeed", 1.f, 1024.f);
	PARSE_FLOAT(config->tileFallYCoord, "tileFallYCoord", -1000.f, 0.f);
	// UI
	PARSE_FLOAT(config->swapThreshold, "swapThreshold", 0.1f, 1.f);
	PARSE_FLOAT(config->startDragThreshold, "startDragThreshold", 1.f, 8.f);
	PARSE_BOOL(config->musicOn, "musicOn");
	PARSE_BOOL(config->sfxOn, "sfxOn");

	config->cellWidthWithSpacing = config->cellWidth + config->cellSpacing;
	config->cellHeightWithSpacing = config->cellHeight + config->cellSpacing;
	return 1;
}

void LoadGameConfig(GameConfig& gameConfig, const char* iniFile) {
	ParseINIFile(iniFile, INIParser, &gameConfig);
}

} // namespace
