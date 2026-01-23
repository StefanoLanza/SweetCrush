#pragma once

#include <engine/FwdDecl.h>
#include <engine/GameScreen.h>
#include <engine/UI.h>

struct MatchStats;

class LevelCompleteScreen final : public Wind::GameScreen {
public:
	LevelCompleteScreen(Wind::Engine& engine, const MatchStats& matchStats);

	const char*        GetName() const override;
	void               LoadAssets() override;
	void               BuildUI(Wind::UICanvas& canvas) override;
	Wind::GameScreenId Tick(float dt, const Wind::Input& input) override;
	void               Draw(Wind::GameScreenId topScreen) const override;
	void               Enter(Wind::GameScreenId prevScreen, const void* payload) override;
	void               Exit(Wind::GameScreenId newScreen) override;
	void               ParseConfig(const char* varName, const char* varValue) override;

public:
	Wind::Engine&     mEngine;
	const MatchStats& mMatchStats;
	Wind::UIText      mTitle;
	Wind::UIButton    mNextLevelButton;
	Wind::UIPanel     mPanel;
};
