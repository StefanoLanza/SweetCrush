#include "Game.h"
#include "Actions.h"
#include "AudioSettingsScreen.h"
#include "Constants.h"
#include "CreditsScreen.h"
#include "GameCompleteScreen.h"
#include "GameConfig.h"
#include "GameOverScreen.h"
#include "GraphicsSettingsScreen.h"
#include "LevelCompleteScreen.h"
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

Game::Game(Engine& engine, const GameRenderer& gameRenderer, const GameConfig& gameConfig, GameDataModule& gameDataModule)
    : mEngine { engine }
    , mGameConfig { gameConfig }
    , mGameDataModule { gameDataModule }
    , mGameSettings {}
    , mFrameBuffer { RefWindowWidth, RefWindowHeight, FBOFlags::color }
    , mMatchStats {}
    , mScreenId { ScreenId::mainMenu } {
	// Note: match order of ScreenId
	mScreens[0] = std::make_unique<MainScreen>(mEngine);
	mScreens[1] = std::make_unique<CreditsScreen>(mEngine);
	mScreens[2] = std::make_unique<SettingsScreen>(mEngine, mGameSettings);
	mScreens[3] = std::make_unique<PlayScreen>(mEngine, gameRenderer, mGameConfig, mGameSettings, mRenderActionMgr, mMatchStats, mGameDataModule);
	mScreens[4] = std::make_unique<GameOverScreen>(mEngine, mMatchStats);
	mScreens[5] = std::make_unique<GameCompleteScreen>(mEngine, mMatchStats);
	mScreens[6] = std::make_unique<PauseGameScreen>(mEngine, mMatchStats);
	mScreens[7] = std::make_unique<LevelCompleteScreen>(mEngine, mMatchStats);
	mScreens[8] = std::make_unique<GraphicsSettingsScreen>(mEngine, mGameSettings);
	mScreens[9] = std::make_unique<AudioSettingsScreen>(mEngine, mGameSettings);
}

Game::~Game() = default;

void Game::Run() {
	SetLanguage(Language::english);

	mGameSettings = mGameConfig.settings;

	mCanvas.SetBackground("gameartguppy/background.png", mEngine.GetGraphics());
#if ! defined(__ANDROID__) && (defined(_WIN32) || defined(__linux__))
	mCanvas.SetMousePointer("cursor.png", mEngine.GetGraphics());
#endif

	for (const auto& gs : mScreens) {
		gs->LoadAssets();
		gs->BuildUI(mCanvas);
	}
	mScreens[0]->Enter(ScreenId::empty);
	mEngine.Start([this](float dt) { Draw(dt); }, [this](float dt) { Tick(dt); });
}

int Game::ParseConfig(void* user, const char* section, const char* name, const char* value) {
	Game* game = static_cast<Game*>(user);
	for (const auto& screen : game->mScreens) {
		if (! strcmp(screen->GetName(), section)) {
			screen->ParseConfig(name, value);
		}
	}
	return 1;
}

void Game::Draw(float dt) {
	const Input&          input = mEngine.GetInput();
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
	mEngine.GetBlitter().Blit(mFrameBuffer, BlitFilter::point);
	graphics.Flush();
}

void Game::Tick(float dt) {
	Input&     input = mEngine.GetInput();
	const Vec2 fbMouseCoord = mEngine.GetBlitter().WindowToFrameBuffer(input.GetMouseCoord(), mFrameBuffer);
	input.SetMappedMouseCoord(fbMouseCoord);
	mCanvas.UpdateWidgets(RefWindowWidth, RefWindowHeight);

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
