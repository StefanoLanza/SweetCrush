#pragma once

#include <engine/FwdDecl.h>
#include <engine/GameScreen.h>
#include <engine/UI.h>

struct GameSettings;

class SettingsScreen final : public Wind::GameScreen {
public:
	SettingsScreen(Wind::Engine& engine, GameSettings& gameSettings);

	const char*        GetName() const override;
	void               LoadAssets() override;
	void               BuildUI(Wind::UICanvas& canvas) override;
	Wind::GameScreenId Tick(float dt, const Wind::Input& input) override;
	void               Draw(Wind::GameScreenId topScreen) const override;
	void               Enter(Wind::GameScreenId prevScreen, const void* payload) override;
	void               Exit(Wind::GameScreenId newScreen) override;
	void               ParseConfig(const char* varName, const char* varValue) override;

private:
	void RefreshLanguageButton();

private:
	GameSettings&  mGameConfig;
	Wind::UIText   mTitle;
	Wind::UIButton mGraphicsButton;
	Wind::UIButton mAudioButton;
	Wind::UIButton mLanguageButton;
	Wind::UIButton mBackButton;
	Wind::UIPanel  mPanel;
};
