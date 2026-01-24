#pragma once

#include <engine/FwdDecl.h>
#include <engine/GameScreen.h>
#include <engine/UI.h>

struct MatchStats;

class LevelCompleteScreen final : public Wind::GameScreen {
public:
	LevelCompleteScreen(Wind::Engine& engine, const MatchStats& matchStats);

	const char*        GetName() const override;
	void               LoadAssets(Wind::Engine& engine) override;
	void               BuildUI(Wind::UICanvas& canvas) override;
	Wind::ScreenTransition Tick(float dt, const Wind::Input& input) override;
	void               Draw(Wind::UIRenderer& uiRenderer) override;
	void               Enter(Wind::ScreenId prevScreen, const void* payload) override;
	void               Exit() override;
	void               ParseConfig(const char* varName, const char* varValue) override;

public:
	Wind::Engine&     mEngine;
	const MatchStats& mMatchStats;
	Wind::UIText      mTitle;
	Wind::UIButton    mNextLevelButton;
	Wind::UIPanel     mPanel;
};
