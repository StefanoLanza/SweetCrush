#pragma once

#include <engine/FwdDecl.h>
#include <engine/GameScreen.h>
#include <engine/UI.h>

struct MatchStats;

class PauseGameScreen final : public Wind::GameScreen {
public:
	PauseGameScreen(Wind::Engine& engine, MatchStats& matchStats);

	void               LoadAssets() override;
	void               BuildUI(Wind::UICanvas& canvas) override;
	Wind::GameScreenId Tick(float dt) override;
	void               Draw(Wind::GameScreenId topScreen) const override;
	void               Enter(Wind::GameScreenId prevScreen) override;
	void               Exit() override;

public:
	Wind::Engine&  mEngine;
	MatchStats&    mMatchStats;
	Wind::UIText   mTitle;
	Wind::UIButton mContinueButton;
	Wind::UIButton mRestartLevelButton;
	Wind::UIButton mExitGameButton;
	Wind::UIPanel  mPanel;
};
