#pragma once

#include <engine/FwdDecl.h>
#include <engine/Screen.h>
#include <engine/UI.h>

struct GameSettings;

class CreditsScreen final : public Wind::Screen {
public:
	explicit CreditsScreen(const GameSettings& gameSettings);

	const char*       GetName() const override;
	void              LoadAssets(Wind::Engine& engine) override;
	Wind::ScreenEvent Tick(float dt, const Wind::Input& input) override;
	void              Draw(Wind::UIRenderer& uiRenderer, float dt) override;
	void              Enter(const Wind::ScreenNavArgs& args) override;
	void              Exit() override;
	void              ParseConfig(const char* varName, const char* varValue) override;

private:
	const GameSettings& mGameSettings;
	Wind::UIButton*     mBackButton = nullptr;
	Wind::UICanvas      mCanvas;
};
