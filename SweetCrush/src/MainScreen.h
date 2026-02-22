#pragma once

#include <engine/FwdDecl.h>
#include <engine/Screen.h>
#include <engine/UI.h>

class GameRenderer;

class MainScreen final : public Wind::Screen {
public:
	explicit MainScreen(Wind::Engine& engine);

	const char*       GetName() const override;
	void              LoadAssets(Wind::Engine& engine) override;
	Wind::ScreenEvent Tick(float dt, const Wind::Input& input) override;
	void              Draw(Wind::UIRenderer& uiRenderer, float dt) override;
	void              Enter(const Wind::ScreenNavArgs& args) override;
	void              Exit() override;
	void              ParseConfig(const char* varName, const char* varValue) override;

private:
	void AnimateUI();

private:
	Wind::Engine&   mEngine;
	Wind::UICanvas  mCanvas;
	Wind::UIButton* mStartButton = nullptr;
	Wind::UIButton* mSettingsButton = nullptr;
	Wind::UIButton* mCreditsButton = nullptr;
#if ! defined(__ANDROID__) && ! defined(__OHOS__)
	Wind::UIButton* mQuitButton = nullptr;
#endif
	Wind::UIPanel* mPastryPanel = nullptr;
	float          mAccumTime;
};
