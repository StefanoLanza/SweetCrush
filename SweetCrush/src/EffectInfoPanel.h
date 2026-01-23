#pragma once

#include "Effects.h"

#include <engine/FwdDecl.h>
#include <engine/GameScreen.h>
#include <engine/UI.h>

class EffectInfoPanel final : public Wind::GameScreen {
public:
	explicit EffectInfoPanel(Wind::Engine& engine);

	const char*        GetName() const override;
	Wind::GameScreenId Tick(float dt, const Wind::Input& input) override;
	void               LoadAssets() override;
	void               BuildUI(Wind::UICanvas& canvas) override;
	void               Draw(Wind::GameScreenId topScreen) const override;
	void               Enter(Wind::GameScreenId prevScreen, const void* payload) override;
	void               Exit(Wind::GameScreenId newScreen) override;
	void               ParseConfig(const char* varName, const char* varValue) override;

private:
	void ShowHelp(EffectType type);

private:
	Wind::UIText   mTitle;
	Wind::UIText   mText;
	Wind::UIButton mOKButton;
	Wind::UIBitmap mEffectIcon;
	Wind::UIPanel  mPanel;
	bool           mShowHelp[NumEffects];
};
