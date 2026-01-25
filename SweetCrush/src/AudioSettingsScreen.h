#pragma once

#include <engine/FwdDecl.h>
#include <engine/Screen.h>
#include <engine/UI.h>

struct GameSettings;

class AudioSettingsScreen final : public Wind::Screen {
public:
	explicit AudioSettingsScreen(GameSettings& gameSettings);

	const char*            GetName() const override;
	void                   LoadAssets(Wind::Engine& engine) override;
	Wind::ScreenTransition Tick(float dt, const Wind::Input& input) override;
	void                   Draw(Wind::UIRenderer& uiRenderer) override;
	void                   Enter(Wind::ScreenId prevScreen, const void* payload) override;
	void                   Exit() override;
	void                   ParseConfig(const char* varName, const char* varValue) override;

private:
	//	void RefreshLanguageButton();
	void RefreshMusicButton();
	void RefreshSfxButton();

private:
	GameSettings&  mGameConfig;
	Wind::UIText   mTitle;
	Wind::UIButton mMusicButton;
	Wind::UIButton mSfxButton;
	Wind::UIButton mBackButton;
	Wind::UICanvas mCanvas;
};
