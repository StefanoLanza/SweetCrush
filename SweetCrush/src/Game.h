#pragma once

#include <engine/ActionMgr.h>
#include <engine/FwdDecl.h>
#include <engine/GameScreen.h>
#include <engine/GlFrameBuffer.h>
#include <engine/UI.h>

#include "GameDataModule.h"
#include "GameSettings.h"
#include "MatchStats.h"
#include "ScreenIds.h"

struct GameConfig;

class Game {
public:
	Game(Wind::Engine& engine, const GameConfig& gameConfig, const GameDataModule& gameDataModule);
	~Game();
	void Run();

private:
	void Draw(float dt);
	void Tick(float dt);

private:
	Wind::Engine&                     mEngine;
	const GameConfig&                 mGameConfig;
	const GameDataModule&             mGameDataModule;
	GameSettings                      mGameSettings;
	Wind::GlFrameBuffer               mFrameBuffer;
	Wind::UICanvas                    mCanvas;
	MatchStats                        mMatchStats;
	Wind::ActionMgr                   mRenderActionMgr;
	Wind::GameScreenId                mScreenId;
	std::unique_ptr<Wind::GameScreen> mScreens[8];
};
