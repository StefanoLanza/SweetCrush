#pragma once

#include <engine/FwdDecl.h>
#include <engine/Screen.h>
#include <engine/UI.h>

struct MatchStats;

class GameCompleteScreen final : public Wind::Screen {
public:
	explicit GameCompleteScreen(const MatchStats& matchStats);

	const char*            GetName() const override;
	void                   LoadAssets(Wind::Engine& engine) override;
	Wind::ScreenEvent Tick(float dt, const Wind::Input& input) override;
	void                   Draw(Wind::UIRenderer& uiRenderer) override;
	void                   Enter(Wind::ScreenId prevScreen, const void* payload) override;
	void                   Exit() override;
	void                   ParseConfig(const char* varName, const char* varValue) override;

public:
	const MatchStats& mMatchStats;
	Wind::UIText      mTitle;
	Wind::UIButton    mContinueButton;
	Wind::UIBitmap    mPanelBitmap;
	Wind::UICanvas    mCanvas;
	Wind::FontPtr     mFont;
};
