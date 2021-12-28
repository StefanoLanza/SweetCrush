#include "Game.h"
#include "Actions.h"
#include "Config.h"
#include "Constants.h"
#include "CreditsScreen.h"
#include "GameCompletePanel.h"
#include "GameOverScreen.h"
#include "LevelCompletePanel.h"
#include "Localization.h"
#include "MainScreen.h"
#include "PauseGameScreen.h"
#include "PlayScreen.h"
#include "ScreenIds.h"
#include "SettingsScreen.h"
#include <cassert>
#include <engine/Blitter.h>
#include <engine/Engine.h>
#include <engine/Graphics.h>
#include <engine/Input.h>

using namespace Wind;

Game::Game(Engine& engine, const GameConfig& gameConfig, const GameDataModule& gameDataModule)
    : mEngine { engine }
    , mGameConfig { gameConfig }
	, mGameDataModule { gameDataModule }
    , mGameSettings {}
    , mFrameBuffer { RefWindowWidth, RefWindowHeight }
    , mScreenId { ScreenId::mainMenu } {
}

Game::~Game() = default;

void Game::Run() {
	SetLanguage(Language::english);

	mGameSettings.audioOn = mGameConfig.audioOn;

	mCanvas.SetBackground("gameartguppy/background.png", mEngine);
#if ! defined(__ANDROID__) && (defined(_WIN32) || defined(__linux__))
	mCanvas.SetMousePointer("cursor.png", mEngine);
#endif

	mScreens[0] = std::make_unique<MainScreen>(mEngine);
	mScreens[1] = std::make_unique<CreditsScreen>(mEngine);
	mScreens[2] = std::make_unique<SettingsScreen>(mEngine, mGameSettings);
	mScreens[3] = std::make_unique<PlayScreen>(mEngine, mGameConfig, mGameSettings, mRenderActionMgr, mMatchStats, mGameDataModule);
	mScreens[4] = std::make_unique<GameOverScreen>(mEngine, mMatchStats);
	mScreens[5] = std::make_unique<GameCompletePanel>(mEngine, mMatchStats);
	mScreens[6] = std::make_unique<PauseGameScreen>(mEngine, mMatchStats);
	mScreens[7] = std::make_unique<LevelCompletePanel>(mEngine, mMatchStats);

	for (const auto& gs : mScreens) {
		gs->LoadAssets();
		gs->BuildUI(mCanvas);
	}
	mCanvas.LoadAssets(mEngine);
	mScreens[0]->Enter(ScreenId::none);
	mEngine.Start([this](float dt) { Draw(dt); }, [this](float dt) { Tick(dt); });
}

void Game::Draw(float dt) {
	const Input& input = mEngine.GetInput();
	const TextRenderer&   textRenderer = mEngine.GetTextRenderer();
	const BitmapRenderer& bitmapRender = mEngine.GetBitmapRenderer();
	Graphics&             graphics = mEngine.GetGraphics();

	graphics.SetFrameBuffer(mFrameBuffer);
	mCanvas.Draw(bitmapRender, textRenderer, input.GetMappedMouseCoord());
	for (const auto& screen : mScreens) {
		screen->Draw(mScreenId);
	}
	mRenderActionMgr.RunActions(dt);

	graphics.SetDefaultFrameBuffer();
	mEngine.GetBlitter().Blit(mFrameBuffer);
}

void Game::Tick(float dt) {
	Input& input = mEngine.GetInput();

	const Vec2 fbMouseCoord = mEngine.GetBlitter().WindowToFrameBuffer(input.GetMouseCoord(), mFrameBuffer);  
	input.SetMappedMouseCoord(fbMouseCoord);
	mCanvas.UpdateWidgets(RefWindowWidth, RefWindowHeight);

	GameScreen&        currScreen = *mScreens[(int)mScreenId];
	const GameScreenId nextScreen = currScreen.Tick(dt, input);
	if (nextScreen != mScreenId) {
		currScreen.Exit();
		mScreens[(int)nextScreen]->Enter(mScreenId);
		mScreenId = nextScreen;
	}
}
