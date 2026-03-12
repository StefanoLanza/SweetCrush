#pragma once

#include <engine/FwdDecl.h>
#include <engine/Screen.h>
#include <engine/UI.h>

class PauseScreen final : public Wind::Screen {
public:
	explicit PauseScreen();

	const char*       GetName() const override;
	void              LoadAssets(Wind::Engine& engine) override;
	Wind::ScreenEvent Tick(float dt, const Wind::Input& input) override;
	void              Draw(Wind::UIRenderer& uiRenderer, float dt) override;
	void              Enter(const Wind::ScreenNavArgs& args) override;
	void              Exit() override;
	void              ParseConfig(const char* varName, const char* varValue) override;

public:
	Wind::UICanvas  mCanvas;
	Wind::UIButton* mRestartLevelButton = nullptr;
	Wind::UIButton* mEndGameButton = nullptr;
	Wind::UIButton* mSettingsButton = nullptr;
	Wind::UIButton* mCloseButton = nullptr;
};
