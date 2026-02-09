#pragma once

#include <engine/FwdDecl.h>
#include <engine/Screen.h>
#include <engine/UI.h>

class CreditsScreen final : public Wind::Screen {
public:
	explicit CreditsScreen();

	const char*       GetName() const override;
	void              LoadAssets(Wind::Engine& engine) override;
	Wind::ScreenEvent Tick(float dt, const Wind::Input& input) override;
	void              Draw(Wind::UIRenderer& uiRenderer, float dt) override;
	void              Enter(const Wind::ScreenNavArgs& args) override;
	void              Exit() override;
	void              ParseConfig(const char* varName, const char* varValue) override;

private:
	Wind::UIText   mTitle;
	Wind::UIText   mCodeBy;
	Wind::UIText   mGraphicsBy;
	Wind::UIText   mMusicBy;
	Wind::UIText   mVersion;
	Wind::UIButton mBackButton;
	Wind::UICanvas mCanvas;
	Wind::UIPanel  mPanel;
};
