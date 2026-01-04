#pragma once

#include <engine/FwdDecl.h>
#include <engine/GameScreen.h>
#include <engine/GlFrameBuffer.h>
#include <engine/UI.h>

#include "ActionMgr.h"
#include "GameDataModule.h"
#include "GameSettings.h"
#include "MatchStats.h"
#include "ScreenIds.h"

struct GameConfig;

class Game final {
public:
	Game(Wind::Engine& engine, const GameConfig& gameConfig, GameDataModule& gameDataModule);
	~Game();
	void Run();
	void LoadConfig();

private:
	void Draw(float dt);
	void Tick(float dt);

private:
	Wind::Engine&                     mEngine;
	const GameConfig&                 mGameConfig;
	GameDataModule&                   mGameDataModule;
	GameSettings                      mGameSettings;
	Wind::GlFrameBuffer               mFrameBuffer;
	Wind::UICanvas                    mCanvas;
	MatchStats                        mMatchStats;
	ActionMgr                         mRenderActionMgr;
	Wind::GameScreenId                mScreenId;
	std::unique_ptr<Wind::GameScreen> mScreens[8];
};
