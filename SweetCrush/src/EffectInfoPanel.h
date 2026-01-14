#pragma once

#include "Effects.h"

#include <engine/FwdDecl.h>
#include <engine/UI.h>

class EffectInfoPanel final {
public:
	explicit EffectInfoPanel(Wind::Engine& engine);

	void BuildUI(Wind::UICanvas& canvas);
	void ShowHelp(EffectType type);
	bool IsVisible() const;
	bool Wait(const Wind::Input& input);
	void Hide();

private:
	bool           mShowBoosterHelp[NumEffects];
	Wind::UIText   mTitle;
	Wind::UIText   mText;
	Wind::UIButton mOKButton;
	Wind::UIBitmap mBoosterIcon;
	Wind::UIBitmap mPanelBitmap;
	Wind::UIPanel  mPanel;
};
