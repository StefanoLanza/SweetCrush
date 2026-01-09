#pragma once

#include <engine/FwdDecl.h>
#include <engine/GameScreen.h>
#include <engine/UI.h>

struct MatchStats;

class GameOverScreen final : public Wind::GameScreen {
public:
	GameOverScreen(Wind::Engine& engine, const MatchStats& matchStats);

	const char*        GetName() const override;
	void               LoadAssets() override;
	void               BuildUI(Wind::UICanvas& canvas) override;
	Wind::GameScreenId Tick(float dt, const Wind::Input& input) override;
	void               Draw(Wind::GameScreenId topScreen) const override;
	void               Enter(Wind::GameScreenId prevScreen) override;
	void               Exit() override;
	void               ParseConfig(const char* varName, const char* varValue) override;

private:
	Wind::Engine&     mEngine;
	const MatchStats& mMatchStats;
	Wind::UIText      mTitle;
	Wind::UIButton    mReplayLevelButton;
	Wind::UIButton    mContinueButton;
	Wind::UIPanel     mPanel;
	Wind::FontPtr     mFont;
};
