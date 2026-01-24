#pragma once

#include <engine/FwdDecl.h>
#include <engine/GameScreen.h>
#include <engine/UI.h>

class PauseGameScreen final : public Wind::GameScreen {
public:
	explicit PauseGameScreen(Wind::Engine& engine);

	const char*            GetName() const override;
	void                   LoadAssets() override;
	void                   BuildUI(Wind::UICanvas& canvas) override;
	Wind::ScreenTransition Tick(float dt, const Wind::Input& input) override;
	void                   Draw(Wind::ScreenId topScreen) const override;
	void                   Enter(Wind::ScreenId prevScreen, const void* payload) override;
	void                   Exit() override;
	void                   ParseConfig(const char* varName, const char* varValue) override;

public:
	Wind::Engine&  mEngine;
	Wind::UIText   mTitle;
	Wind::UIButton mContinueButton;
	Wind::UIButton mRestartLevelButton;
	Wind::UIButton mExitGameButton;
	Wind::UIPanel  mPanel;
};
