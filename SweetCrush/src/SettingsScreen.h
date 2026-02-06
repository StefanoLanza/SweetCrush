#pragma once

#include <engine/FwdDecl.h>
#include <engine/Screen.h>
#include <engine/UI.h>

struct GameSettings;

class SettingsScreen final : public Wind::Screen {
public:
	explicit SettingsScreen(GameSettings& gameSettings);

	const char*            GetName() const override;
	void                   LoadAssets(Wind::Engine& engine) override;
	Wind::ScreenEvent Tick(float dt, const Wind::Input& input) override;
	void                   Draw(Wind::UIRenderer& uiRenderer, float dt) override;
	void                   Enter(const Wind::ScreenNavArgs& args) override;
	void                   Exit() override;
	void                   ParseConfig(const char* varName, const char* varValue) override;

private:
	GameSettings&  mGameConfig;
	Wind::UIText   mTitle;
	Wind::UIButton mGraphicsButton;
	Wind::UIButton mAudioButton;
	Wind::UIButton mLanguageButton;
	Wind::UIButton mBackButton;
	Wind::UICanvas mCanvas;
};
