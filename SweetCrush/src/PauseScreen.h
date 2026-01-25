#pragma once

#include <engine/FwdDecl.h>
#include <engine/GameScreen.h>
#include <engine/UI.h>

class PauseScreen final : public Wind::Screen {
public:
	explicit PauseScreen(Wind::Engine& engine);

	const char*            GetName() const override;
	void                   LoadAssets(Wind::Engine& engine) override;
	Wind::ScreenTransition Tick(float dt, const Wind::Input& input) override;
	void                   Draw(Wind::UIRenderer& uiRenderer) override;
	void                   Enter(Wind::ScreenId prevScreen, const void* payload) override;
	void                   Exit() override;
	void                   ParseConfig(const char* varName, const char* varValue) override;

public:
	Wind::Engine&  mEngine;
	Wind::UIText   mTitle;
	Wind::UIButton mContinueButton;
	Wind::UIButton mRestartLevelButton;
	Wind::UIButton mExitGameButton;
	Wind::UICanvas mCanvas;
};
