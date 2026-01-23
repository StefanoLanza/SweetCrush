#pragma once

#include <engine/FwdDecl.h>
#include <engine/GameScreen.h>
#include <engine/UI.h>

class CreditsScreen final : public Wind::GameScreen {
public:
	explicit CreditsScreen(Wind::Engine& engine);

	const char*        GetName() const override;
	void               LoadAssets() override;
	void               BuildUI(Wind::UICanvas& canvas) override;
	Wind::GameScreenId Tick(float dt, const Wind::Input& input) override;
	void               Draw(Wind::GameScreenId topScreen) const override;
	void               Enter(Wind::GameScreenId prevScreen, const void* payload) override;
	void               Exit(Wind::GameScreenId newScreen) override;
	void               ParseConfig(const char* varName, const char* varValue) override;

private:
	Wind::UIText   mTitle;
	Wind::UIText   mCodeBy;
	Wind::UIText   mGraphicsBy;
	Wind::UIText   mMusicBy;
	Wind::UIText   mVersion;
	Wind::UIButton mBackButton;
	Wind::UIPanel  mPanel;
};
