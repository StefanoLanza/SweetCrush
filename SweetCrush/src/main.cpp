#include <SDL3/SDL_main.h>
#include <engine/Engine.h>
#include <engine/IniParser.h>
#include <engine/Sdl.h>
#include <engine/SdlWindow.h>

#include "Game.h"
#include "AppConfig.h"
#include "GameDataModule.h"
#include "GameRenderer.h"

#include <algorithm>
#include <cstring>

namespace {

void LoadAppConfig(AppConfig& gameConfig, const char* iniFile);

} // namespace

int main(int argc, char* argv[]) {
	AppConfig gameConfig = DefaultGameConfig();
	LoadAppConfig(gameConfig, ASSETS_FOLDER "game.ini");

	GameDataModule gameDataModule;
#ifdef _WIN32
	const char* dllName = "gameData.dll";
	if (! gameDataModule.Init(dllName)) {
		return 0;
	}
#elif defined(__linux__)
	const char* dllName = "./libgameData.so";
	if (! gameDataModule.Init(dllName)) {
		return 0;
	}
#endif
	Wind::Sdl       sdl { SDL_INIT_VIDEO | SDL_INIT_EVENTS };
	Wind::SdlWindow window { "SweetCrush", gameConfig.windowWidth, gameConfig.windowHeight, nullptr, gameConfig.fullscreen };
	Wind::Engine    engine { window };
	GameRenderer    gameRenderer { engine };
	Wind::INIParser parser;
	Game            game { engine, gameRenderer, gameConfig, gameDataModule, parser };
	parser.ParseFile(ASSETS_FOLDER "game.ini");
	// SDL bug: set icon *after* all systems have been initialized, to show it correctly in the task manager on Windows
	// https://discourse.libsdl.org/t/sdl-setwindowicon-not-setting-taskbar-icon/62049/19
	window.SetIcon(ASSETS_FOLDER "icon.png");
	game.Run();

	return 0;
}

namespace {

int ParseAppConfig(void* user, const char* /*section*/, const char* name, const char* value) {
	auto config = static_cast<AppConfig*>(user);
	PARSE_INT(config->windowWidth, "windowWidth", 0, 3456);
	PARSE_INT(config->windowHeight, "windowHeight", 0, 2234);
	PARSE_BOOL(config->fullscreen, "fullscreen");
	PARSE_FLOAT(config->board.cellHeight, "cellHeight", 16.f, 64.f);
	PARSE_FLOAT(config->board.cellWidth, "cellWidth", 16.f, 64.f);
	PARSE_FLOAT(config->board.cellSpacing, "cellSpacing", 0.f, 8.f);
	// Animations
	PARSE_FLOAT(config->moveBackPieceDuration, "moveBackPieceSpeed", 0.f, 4.f);
	PARSE_FLOAT(config->swapSpeed, "swapSpeed", 0.f, 4.f);
	PARSE_FLOAT(config->pieceFallDuration, "pieceFallDuration", 0.f, 4.f);
	PARSE_FLOAT(config->pieceFallYCoord, "pieceFallYCoord", -1000.f, 0.f);
	// UI
	PARSE_FLOAT(config->ui.swapThreshold, "swapThreshold", 0.1f, 1.f);
	PARSE_FLOAT(config->ui.startDragThreshold, "startDragThreshold", 1.f, 8.f);
	// Misc
	PARSE_BOOL(config->settings.musicOn, "musicOn");
	PARSE_BOOL(config->settings.sfxOn, "sfxOn");
	PARSE_BOOL(config->settings.infoOn, "infoOn");

	config->board.cellWidthWithSpacing = config->board.cellWidth + config->board.cellSpacing;
	config->board.cellHeightWithSpacing = config->board.cellHeight + config->board.cellSpacing;
	return 1;
}

void LoadAppConfig(AppConfig& gameConfig, const char* iniFile) {
	Wind::ParseINIFile(iniFile, ParseAppConfig, &gameConfig);
}

} // namespace
