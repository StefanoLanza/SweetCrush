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
#include "LevelCompleteScreen.h"
#include "Localization.h"
#include "MainScreen.h"
#include "PauseGameScreen.h"
#include "PlayScreen.h"
#include "SettingsScreen.h"

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
    , mScreenStack { GameScreenIds::mainMenu, GameScreenIds::mainMenu }
    , mScreenStackSize { 1 } {
	// Note: match order of GameScreenId
	mScreens[0] = std::make_unique<MainScreen>(mEngine, gameRenderer);
	mScreens[1] = std::make_unique<CreditsScreen>(mEngine);
	mScreens[2] = std::make_unique<SettingsScreen>(mEngine, mGameSettings);
	mScreens[3] = std::make_unique<PlayScreen>(mEngine, gameRenderer, mGameConfig, mGameSettings, mRenderActionMgr, mMatchStats, mGameDataModule);
	mScreens[4] = std::make_unique<GameOverScreen>(mEngine, mMatchStats);
	mScreens[5] = std::make_unique<GameCompleteScreen>(mEngine, mMatchStats);
	mScreens[6] = std::make_unique<PauseGameScreen>(mEngine);
	mScreens[7] = std::make_unique<LevelCompleteScreen>(mEngine, mMatchStats);
	mScreens[8] = std::make_unique<GraphicsSettingsScreen>(mEngine, mGameSettings);
	mScreens[9] = std::make_unique<AudioSettingsScreen>(mEngine, mGameSettings);
	mScreens[10] = std::make_unique<EffectInfoPanel>(mEngine);

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

	mScreens[0]->Enter(GameScreenIds::mainMenu, nullptr);
	mEngine.Start([this](float dt) { Draw(dt); }, [this](float dt) { Tick(dt); });
}

void Game::Draw(float dt) {
	const Input&        input = mEngine.GetInput();
	const TextRenderer& textRenderer = mEngine.GetTextRenderer();
	Graphics&           graphics = mEngine.GetGraphics();

	graphics.SetFrameBuffer(mFrameBuffer);
	mCanvas.Draw(RefWindowWidth, RefWindowHeight, mUIRenderer, textRenderer, input.GetMappedMouseCoord());

	for (size_t i = 0; i < mScreenStackSize; ++i) {
		mScreens[mScreenStack[i].Get()]->Draw(mScreenStack[mScreenStackSize - 1]);
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

	const ScreenId         topScreenId = mScreenStack[mScreenStackSize - 1];
	GameScreen&            topScreen = *mScreens[topScreenId.Get()];
	const ScreenTransition transition = topScreen.Tick(dt, input);
	switch (transition.mOp) {
	case ScreenOp::keep:
		break;
	case ScreenOp::pop:
		assert(mScreenStackSize == 2);
		topScreen.Exit();
		mScreenStackSize--;
		break;
	case ScreenOp::replace:
		for (size_t i = 0; i < mScreenStackSize; ++i) {
			mScreens[mScreenStack[i].Get()]->Exit();
		}
		mScreens[transition.mNext.Get()]->Enter(topScreenId, transition.mPayload);
		mScreenStack[0] = transition.mNext;
		mScreenStackSize = 1;
		break;
	case ScreenOp::push:
		assert(mScreenStackSize == 1);
		mScreens[transition.mNext.Get()]->Enter(topScreenId, transition.mPayload);
		mScreenStack[1] = transition.mNext;
		mScreenStackSize++;
		break;
	};
}
