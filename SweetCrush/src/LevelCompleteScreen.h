#pragma once

#include <engine/FwdDecl.h>
#include <engine/Screen.h>
#include <engine/UI.h>

struct MatchStats;

class LevelCompleteScreen final : public Wind::Screen {
public:
	explicit LevelCompleteScreen(const MatchStats& matchStats);

	const char*       GetName() const override;
	void              LoadAssets(Wind::Engine& engine) override;
	Wind::ScreenEvent Tick(float dt, const Wind::Input& input) override;
	void              Draw(Wind::UIRenderer& uiRenderer, float dt) override;
	void              Enter(const Wind::ScreenNavArgs& args) override;
	void              Exit() override;
	void              ParseConfig(const char* varName, const char* varValue) override;

private:
	const MatchStats& mMatchStats;
	Wind::UIText      mTitle;
	Wind::UIButton    mNextLevelButton;
	Wind::UICanvas    mCanvas;
	float             mAccumTime = 0.f;
};
