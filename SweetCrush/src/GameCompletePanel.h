#pragma once

#include <engine/FwdDecl.h>
#include <engine/GameScreen.h>
#include <engine/UI.h>

struct MatchStats;

class GameCompletePanel final : public Wind::GameScreen {
public:
	GameCompletePanel(Wind::Engine& engine, const MatchStats& matchStats);

	void               LoadAssets() override;
	void               BuildUI(Wind::UICanvas& canvas) override;
	Wind::GameScreenId Tick(float dt, const Wind::Input& input) override;
	void               Draw(Wind::GameScreenId topScreen) const override;
	void               Enter(Wind::GameScreenId prevScreen) override;
	void               Exit() override;

public:
	Wind::Engine&     mEngine;
	const MatchStats& mMatchStats;
	Wind::UIText      mTitle;
	Wind::UIButton    mContinueButton;
	Wind::UIBitmap    mPanelBitmap;
	Wind::UIPanel     mPanel;
	Wind::FontPtr     mFont;
};
