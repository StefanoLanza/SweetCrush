#pragma once

#include <engine/FwdDecl.h>
#include <engine/GameScreen.h>
#include <engine/UI.h>

class MainScreen final : public Wind::GameScreen {
public:
	MainScreen(Wind::Engine& engine);

	void               LoadAssets() override;
	void               BuildUI(Wind::UICanvas& canvas) override;
	Wind::GameScreenId Tick(float dt) override;
	void               Draw(Wind::GameScreenId topScreen) const override;
	void               Enter(Wind::GameScreenId prevScreen) override;
	void               Exit() override;

private:
	Wind::Engine&  mEngine;
	Wind::UIText   mTitle;
	Wind::UIButton mStartButton;
	Wind::UIButton mSettingsButton;
	Wind::UIButton mCreditsButton;
	Wind::UIButton mQuitButton;
	Wind::UIBitmap mPanelBitmap;
	Wind::UIPanel  mPanel;
	float          mTime;
};
