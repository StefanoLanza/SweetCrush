#pragma once

#include <engine/FwdDecl.h>
#include <engine/GameScreen.h>
#include <engine/GlFrameBuffer.h>
#include <engine/UI.h>
#include <engine/ActionMgr.h>

#include "GameSettings.h"
#include "ScreenIds.h"
#include "MatchStats.h"

using namespace Wind;

struct GameConfig;

class Game {
public:
	Game(Engine& engine, const GameConfig& gameConfig);
	~Game();
	void Run();

private:
	void Draw(float dt);
	void Tick(float dt);

private:
	Engine&                     mEngine;
	const GameConfig&           mGameConfig;
	GameSettings                mGameSettings;
	GlFrameBuffer               mFrameBuffer;
	UICanvas                    mCanvas;
	MatchStats                  mMatchStats;
	Wind::ActionMgr             mRenderActionMgr;
	GameScreenId                mScreenId;
	std::unique_ptr<GameScreen> mScreens[8];
};
