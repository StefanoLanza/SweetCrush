#pragma once

#include <engine/ActionMgr.h>
#include <engine/FwdDecl.h>
#include <engine/GlFrameBuffer.h>
#include <engine/Kawase.h> // FIXME
#include <engine/Screen.h>
#include <engine/ScreenManager.h>
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
	Wind::Engine&                 mEngine;
	const AppConfig&              mGameConfig;
	GameDataModule&               mGameDataModule;
	GameSettings                  mGameSettings;
	MatchStats                    mMatchStats;
	Wind::GlFrameBuffer           mFrameBuffer_0;
	Wind::GlFrameBuffer           mFrameBuffer_1;
	Wind::GlFrameBuffer           mFrameBufferHalfRes;
	Wind::GlFrameBuffer           mFrameBufferQuarterRes;
	Wind::UIMouseCursor           mMouseCursor;
	Wind::UIRenderer              mUIRenderer;
	Wind::ScreenManager           mScreenMgr;
	Wind::KawaseBlur              mBlur;
	std::unique_ptr<Wind::Screen> mScreens[13];
};
