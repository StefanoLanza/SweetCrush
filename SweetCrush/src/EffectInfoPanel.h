#pragma once

#include "Effects.h"

#include <engine/FwdDecl.h>
#include <engine/Screen.h>
#include <engine/UI.h>

class EffectInfoScreen final : public Wind::Screen {
public:
	explicit EffectInfoScreen();

	const char*       GetName() const override;
	Wind::ScreenEvent Tick(float dt, const Wind::Input& input) override;
	void              LoadAssets(Wind::Engine& engine) override;
	void              Draw(Wind::UIRenderer& uiRenderer, float dt) override;
	void              Enter(const Wind::ScreenNavArgs& args) override;
	void              Exit() override;
	void              ParseConfig(const char* varName, const char* varValue) override;

private:
	void ShowHelp(EffectType type);

private:
	Wind::UIText*   mTitle = nullptr;
	Wind::UIText*   mText;
	Wind::UIButton* mOKButton;
	Wind::UIBitmap* mEffectIcon;
	bool            mShowHelp[NumEffects];
};
