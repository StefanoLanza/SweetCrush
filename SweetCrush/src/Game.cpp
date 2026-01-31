#include "Game.h"
#include "Actions.h"
#include "AppConfig.h"
#include "Constants.h"
#include "ScreenIds.h"

#include <engine/Blitter.h>
#include <engine/Engine.h>
#include <engine/Graphics.h>
#include <engine/IniParser.h>
#include <engine/Input.h>

// Game screens
#include "AudioSettingsScreen.h"
#include "CreditsScreen.h"
#include "EffectInfoPanel.h"
#include "GameCompleteScreen.h"
#include "GameDrawOrder.h"
#include "GameOverScreen.h"
#include "GraphicsSettingsScreen.h"
#include "LanguageScreen.h"
#include "LevelCompleteScreen.h"
#include "LevelStartScreen.h"
#include "Localization.h"
#include "MainScreen.h"
#include "PauseScreen.h"
#include "PlayScreen.h"
#include "SettingsScreen.h"

#include <cassert>

using namespace Wind;

Game::Game(Engine& engine, const GameRenderer& gameRenderer, const AppConfig& gameConfig, GameDataModule& gameDataModule, Wind::INIParser& iniParser)
    : mEngine { engine }
    , mGameConfig { gameConfig }
    , mGameDataModule { gameDataModule }
    , mGameSettings {}
    , mMatchStats {}
    , mCompositor { engine.GetGraphics(), RefWindowWidth, RefWindowHeight }
    , mUIRenderer { engine.GetGraphics(), engine.GetTextRenderer() } {
	// Note: match order of GameScreenId
	mScreens[0] = std::make_unique<MainScreen>(engine, gameRenderer);
	mScreens[1] = std::make_unique<CreditsScreen>();
	mScreens[2] = std::make_unique<SettingsScreen>(mGameSettings);
	mScreens[3] = std::make_unique<PlayScreen>(engine, gameRenderer, gameConfig, mGameSettings, mMatchStats, gameDataModule);
	mScreens[4] = std::make_unique<GameOverScreen>(mMatchStats);
	mScreens[5] = std::make_unique<GameCompleteScreen>(mMatchStats);
	mScreens[6] = std::make_unique<PauseScreen>();
	mScreens[7] = std::make_unique<LevelCompleteScreen>(mMatchStats);
	mScreens[8] = std::make_unique<GraphicsSettingsScreen>(mGameSettings);
	mScreens[9] = std::make_unique<AudioSettingsScreen>(mGameSettings);
	mScreens[10] = std::make_unique<EffectInfoScreen>();
	mScreens[11] = std::make_unique<LanguageScreen>();
	mScreens[12] = std::make_unique<LevelStartScreen>(mMatchStats, gameDataModule, gameRenderer);

	for (const auto& screen : mScreens) {
		iniParser.AddListener(screen->GetName(),
		                      [screenPtr = screen.get()](const char* varName, const char* varValue) { screenPtr->ParseConfig(varName, varValue); });
	}
}

Game::~Game() = default;

void Game::Run() {
	SetLanguage(Language::english);

	mGameSettings = mGameConfig.settings;

#if ! defined(__ANDROID__) && ! defined(__OHOS__)
	mMouseCursor.SetCursor("cursor.png", mEngine.GetGraphics());
#endif
	for (const auto& screen : mScreens) {
		screen->LoadAssets(mEngine);
		mScreenMgr.Register(screen.get());
	}

	mScreenMgr.SetMain(GameScreenIds::mainMenu);
	mEngine.Start([this](float dt) { Draw(dt); }, [this](float dt) { Tick(dt); });
}

void Game::Draw(float dt) {
	const Input& input = mEngine.GetInput();
	Graphics&    graphics = mEngine.GetGraphics();

	graphics.SetFrameBuffer(mCompositor.GetWriteableFramebuffer());

	mScreenMgr.Draw(mUIRenderer, dt);
#if ! defined(__ANDROID__) && ! defined(__OHOS__)
	mMouseCursor.Draw(mUIRenderer, input.GetMappedMouseCoord(), GameDrawOrder::mousePointer);
#endif
	const GlFrameBuffer& compositedFB = mCompositor.Composite(mScreenMgr.GetTransition(), dt);
	graphics.SetDefaultFrameBuffer();
	mEngine.GetBlitter().Blit(compositedFB, BlitFilter::point);

	graphics.Flush();
}

void Game::Tick(float dt) {
	Input&     input = mEngine.GetInput();
	const Vec2 fbMouseCoord = mEngine.GetBlitter().WindowToFrameBuffer(input.GetMouseCoord(), mCompositor.GetWriteableFramebuffer());
	input.SetMappedMouseCoord(fbMouseCoord);

	mGameDataModule.Reload();
	if (! mGameDataModule.IsValid()) {
		return;
	}
	mScreenMgr.Tick(dt, input);
}
