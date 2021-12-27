#pragma once

#include <engine/FwdDecl.h>
#include <engine/GameScreen.h>
#include <engine/UI.h>

class CreditsScreen final : public Wind::GameScreen {
public:
	CreditsScreen(Wind::Engine& engine);

	void               LoadAssets() override;
	void               BuildUI(Wind::UICanvas& canvas) override;
	Wind::GameScreenId Tick(float dt, const Wind::Input& input) override;
	void               Draw(Wind::GameScreenId topScreen) const override;
	void               Enter(Wind::GameScreenId prevScreen) override;
	void               Exit() override;

private:
	Wind::Engine&  mEngine;
	Wind::UIText   mTitle;
	Wind::UIText   mCodeBy;
	Wind::UIText   mGraphicsBy;
	Wind::UIText   mMusicBy;
	Wind::UIButton mBackButton;
	Wind::UIPanel  mPanel;
};
