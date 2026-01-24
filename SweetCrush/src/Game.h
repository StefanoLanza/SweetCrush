#pragma once

#include <engine/ActionMgr.h>
#include <engine/FwdDecl.h>
#include <engine/GameScreen.h>
#include <engine/GlFrameBuffer.h>
#include <engine/UI.h>
#include <engine/UIRenderer.h>

#include "GameDataModule.h"
#include "GameSettings.h"
#include "MatchStats.h"
#include "ScreenIds.h"

struct AppConfig;
class GameRenderer;

class Game final {
public:
	Game(Wind::Engine& engine, const GameRenderer& gameRenderer, const AppConfig& gameConfig, GameDataModule& gameDataModule,
	     Wind::INIParser& iniParser);
	~Game();
	void Run();

private:
	void Draw(float dt);
	void Tick(float dt);

private:
	Wind::Engine&                     mEngine;
	const AppConfig&                  mGameConfig;
	GameDataModule&                   mGameDataModule;
	GameSettings                      mGameSettings;
	Wind::GlFrameBuffer               mFrameBuffer;
	Wind::UICanvas                    mCanvas;
	Wind::UIRenderer                  mUIRenderer;
	MatchStats                        mMatchStats;
	Wind::ActionMgr                   mRenderActionMgr;
	Wind::ScreenId                    mScreenStack[2];
	int                               mScreenStackSize;
	std::unique_ptr<Wind::GameScreen> mScreens[11];
};
