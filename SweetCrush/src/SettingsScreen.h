#pragma once

#include <engine/FwdDecl.h>
#include <engine/GameScreen.h>
#include <engine/UI.h>

struct GameSettings;

class SettingsScreen final : public Wind::GameScreen {
public:
	SettingsScreen(Wind::Engine& engine, GameSettings& gameSettings);

	void               LoadAssets() override;
	void               BuildUI(Wind::UICanvas& canvas) override;
	Wind::GameScreenId Tick(float dt, const Wind::Input& input) override;
	void               Draw(Wind::GameScreenId topScreen) const override;
	void               Enter(Wind::GameScreenId prevScreen) override;
	void               Exit() override;

private:
	void RefreshLanguageButton();
	void RefreshAudioButton();

private:
	GameSettings&  mGameConfig;
	Wind::UIText   mTitle;
	Wind::UIButton mLanguageButton;
	Wind::UIButton mAudioButton;
	Wind::UIButton mBackButton;
	Wind::UIPanel  mPanel;
};
