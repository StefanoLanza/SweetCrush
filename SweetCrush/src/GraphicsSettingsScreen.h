#pragma once

#include <engine/FwdDecl.h>
#include <engine/GameScreen.h>
#include <engine/UI.h>

struct GameSettings;

class GraphicsSettingsScreen final : public Wind::GameScreen {
public:
	GraphicsSettingsScreen(Wind::Engine& engine, GameSettings& gameSettings);

	const char*        GetName() const override;
	void               LoadAssets(Wind::Engine& engine) override;
	void               BuildUI(Wind::UICanvas& canvas) override;
	Wind::ScreenTransition Tick(float dt, const Wind::Input& input) override;
	void               Draw(Wind::UIRenderer& uiRenderer) override;
	void               Enter(Wind::ScreenId prevScreen, const void* payload) override;
	void               Exit() override;
	void               ParseConfig(const char* varName, const char* varValue) override;

private:
//	void RefreshLanguageButton();

private:
	GameSettings&  mGameConfig;
	Wind::UIText   mTitle;
	Wind::UIButton mFilterButton;
	Wind::UIButton mBackButton;
	Wind::UICanvas mCanvas;
};
