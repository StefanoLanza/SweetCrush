#pragma once

#include <engine/FwdDecl.h>
#include <engine/GameScreen.h>
#include <engine/UI.h>

class GameRenderer;

class MainScreen final : public Wind::GameScreen {
public:
	explicit MainScreen(Wind::Engine& engine, const GameRenderer& gameRenderer);

	const char*        GetName() const override;
	void               LoadAssets() override;
	void               BuildUI(Wind::UICanvas& canvas) override;
	Wind::GameScreenId Tick(float dt, const Wind::Input& input) override;
	void               Draw(Wind::GameScreenId topScreen) const override;
	void               Enter(Wind::GameScreenId prevScreen) override;
	void               Exit() override;
	void               ParseConfig(const char* varName, const char* varValue) override;

private:
	Wind::Engine&       mEngine;
	const GameRenderer& mGameRenderer;
	Wind::UIText        mTitle;
	Wind::UIButton      mStartButton;
	Wind::UIButton      mSettingsButton;
	Wind::UIButton      mCreditsButton;
#if ! defined(__ANDROID__) && ! defined(__OHOS__)
	Wind::UIButton mQuitButton;
#endif
	Wind::UIPanel mPanel;
	float         mTime;
};
