#pragma once

#include <engine/FwdDecl.h>
#include <engine/GameScreen.h>
#include <engine/UI.h>

class LeaveGamePanel final : public Wind::GameScreen {
public:
	LeaveGamePanel(Wind::Engine& engine);

	void               LoadAssets() override;
	void               BuildUI(Wind::UICanvas& canvas) override;
	Wind::GameScreenId Tick(float dt) override;
	void               Draw(Wind::GameScreenId topScreen) const override;
	void               Enter(Wind::GameScreenId prevScreen) override;
	void               Exit() override;

public:
	Wind::Engine&  mEngine;
	Wind::UIText   mTitle;
	Wind::UIButton mYesButton;
	Wind::UIButton mNoButton;
	Wind::UIBitmap mPanelBitmap;
	Wind::UIPanel  mPanel;
};
