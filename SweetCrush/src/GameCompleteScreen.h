#pragma once

#include <engine/FwdDecl.h>
#include <engine/Screen.h>
#include <engine/UI.h>

struct MatchStats;

class GameCompleteScreen final : public Wind::Screen {
public:
	explicit GameCompleteScreen(const MatchStats& matchStats);

	const char*       GetName() const override;
	void              LoadAssets(Wind::Engine& engine) override;
	Wind::ScreenEvent Tick(float dt, const Wind::Input& input) override;
	void              Draw(Wind::UIRenderer& uiRenderer, float dt) override;
	void              Enter(const Wind::ScreenNavArgs& args) override;
	void              Exit() override;
	void              ParseConfig(const char* varName, const char* varValue) override;

private:
	const MatchStats& mMatchStats;
	Wind::UIButton    mContinueButton;
	Wind::UICanvas    mCanvas;
	Wind::UIText      mText0;
	Wind::UIText      mText1;
	float             mAccumTime = 0.f;
};
