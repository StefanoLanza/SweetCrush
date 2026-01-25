#pragma once

#include <engine/FwdDecl.h>
#include <engine/GameScreen.h>
#include <engine/UI.h>

class GameRenderer;

class MainScreen final : public Wind::Screen {
public:
	explicit MainScreen(Wind::Engine& engine, const GameRenderer& gameRenderer);

	const char*            GetName() const override;
	void                   LoadAssets(Wind::Engine& engine) override;
	Wind::ScreenTransition Tick(float dt, const Wind::Input& input) override;
	void                   Draw(Wind::UIRenderer& uiRenderer) override;
	void                   Enter(Wind::ScreenId prevScreen, const void* payload) override;
	void                   Exit() override;
	void                   ParseConfig(const char* varName, const char* varValue) override;

private:
	Wind::Engine&       mEngine;
	const GameRenderer& mGameRenderer;
	Wind::UICanvas      mCanvas;
	Wind::UIText        mTitle;
	Wind::UIButton      mStartButton;
	Wind::UIButton      mSettingsButton;
	Wind::UIButton      mCreditsButton;
#if ! defined(__ANDROID__) && ! defined(__OHOS__)
	Wind::UIButton mQuitButton;
#endif
	float mTime;
};
