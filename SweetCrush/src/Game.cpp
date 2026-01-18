#include "Game.h"
#include "Actions.h"
#include "AudioSettingsScreen.h"
#include "Constants.h"
#include "CreditsScreen.h"
#include "GameCompleteScreen.h"
#include "AppConfig.h"
#include "GameOverScreen.h"
#include "GraphicsSettingsScreen.h"
#include "LevelCompleteScreen.h"
#include "Localization.h"
#include "MainScreen.h"
#include "PauseGameScreen.h"
#include "PlayScreen.h"
#include "ScreenIds.h"
#include "SettingsScreen.h"
#include <engine/Blitter.h>
#include <engine/Engine.h>
#include <engine/Graphics.h>
#include <engine/IniParser.h>
#include <engine/Input.h>

#include <cassert>

using namespace Wind;

Game::Game(Engine& engine, const GameRenderer& gameRenderer, const AppConfig& gameConfig, GameDataModule& gameDataModule, Wind::INIParser& iniParser)
    : mEngine { engine }
    , mGameConfig { gameConfig }
    , mGameDataModule { gameDataModule }
    , mGameSettings {}
    , mFrameBuffer { RefWindowWidth, RefWindowHeight, FBOFlags::color }
    , mUIRenderer { engine.GetGraphics() }
    , mMatchStats {}
    , mScreenId { ScreenId::mainMenu } {
	// Note: match order of ScreenId
	mScreens[0] = std::make_unique<MainScreen>(mEngine, gameRenderer);
	mScreens[1] = std::make_unique<CreditsScreen>(mEngine);
	mScreens[2] = std::make_unique<SettingsScreen>(mEngine, mGameSettings);
	mScreens[3] = std::make_unique<PlayScreen>(mEngine, gameRenderer, mGameConfig, mGameSettings, mRenderActionMgr, mMatchStats, mGameDataModule);
	mScreens[4] = std::make_unique<GameOverScreen>(mEngine, mMatchStats);
	mScreens[5] = std::make_unique<GameCompleteScreen>(mEngine, mMatchStats);
	mScreens[6] = std::make_unique<PauseGameScreen>(mEngine, mMatchStats);
	mScreens[7] = std::make_unique<LevelCompleteScreen>(mEngine, mMatchStats);
	mScreens[8] = std::make_unique<GraphicsSettingsScreen>(mEngine, mGameSettings);
	mScreens[9] = std::make_unique<AudioSettingsScreen>(mEngine, mGameSettings);

	for (const auto& screen : mScreens) {
		iniParser.AddListener(screen->GetName(),
		                      [screenPtr = screen.get()](const char* varName, const char* varValue) { screenPtr->ParseConfig(varName, varValue); });
	}
}

Game::~Game() = default;

void Game::Run() {
	SetLanguage(Language::english);

	mGameSettings = mGameConfig.settings;

	mCanvas.SetBackground("gameartguppy/background.png", mEngine.GetGraphics());
#if ! defined(__ANDROID__) && ! defined(__OHOS__) && ((defined(_WIN32) || defined(__linux__)))
	mCanvas.SetMousePointer("cursor.png", mEngine.GetGraphics());
#endif
	for (const auto& screen : mScreens) {
		screen->LoadAssets();
		screen->BuildUI(mCanvas);
	}
	mCanvas.LoadGraphics(mEngine.GetGraphics());

	mScreens[0]->Enter(ScreenId::empty);
	mEngine.Start([this](float dt) { Draw(dt); }, [this](float dt) { Tick(dt); });
}

void Game::Draw(float dt) {
	const Input&        input = mEngine.GetInput();
	const TextRenderer& textRenderer = mEngine.GetTextRenderer();
	Graphics&           graphics = mEngine.GetGraphics();

	graphics.SetFrameBuffer(mFrameBuffer);
	mCanvas.Draw(RefWindowWidth, RefWindowHeight, mUIRenderer, textRenderer, input.GetMappedMouseCoord());
	for (const auto& screen : mScreens) {
		screen->Draw(mScreenId);
	}
	mRenderActionMgr.RunActions(dt);

	graphics.SetDefaultFrameBuffer();
	mEngine.GetBlitter().Blit(mFrameBuffer, BlitFilter::point);
	graphics.Flush();
}

void Game::Tick(float dt) {
	Input&     input = mEngine.GetInput();
	const Vec2 fbMouseCoord = mEngine.GetBlitter().WindowToFrameBuffer(input.GetMouseCoord(), mFrameBuffer);
	input.SetMappedMouseCoord(fbMouseCoord);

	mGameDataModule.Reload();
	if (! mGameDataModule.IsValid()) {
		return;
	}

	GameScreen&        currScreen = *mScreens[(int)mScreenId];
	const GameScreenId nextScreen = currScreen.Tick(dt, input);
	if (nextScreen != mScreenId) {
		currScreen.Exit();
		mScreens[(int)nextScreen]->Enter(mScreenId);
		mScreenId = nextScreen;
	}
}
