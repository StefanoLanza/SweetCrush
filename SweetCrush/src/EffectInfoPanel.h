#pragma once

#include "Effects.h"

#include <engine/FwdDecl.h>
#include <engine/Screen.h>
#include <engine/UI.h>

class EffectInfoScreen final : public Wind::Screen {
public:
	explicit EffectInfoScreen();

	const char*            GetName() const override;
	Wind::ScreenEvent Tick(float dt, const Wind::Input& input) override;
	void                   LoadAssets(Wind::Engine& engine) override;
	void                   Draw(Wind::UIRenderer& uiRenderer) override;
	void                   Enter(Wind::ScreenId prevScreen, const void* payload) override;
	void                   Exit() override;
	void                   ParseConfig(const char* varName, const char* varValue) override;

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
