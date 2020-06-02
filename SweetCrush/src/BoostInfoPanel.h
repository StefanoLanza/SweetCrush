#pragma once

#include "Board.h"
#include <engine/FwdDecl.h>
#include <engine/UI.h>

class BoostInfoPanel {
public:
	BoostInfoPanel(Wind::Engine& engine);

	void BuildUI(Wind::UICanvas& canvas);
	void ShowHelp(BoosterType type);
	bool IsVisible() const;
	bool Wait(const Wind::Input& input);
	void Hide();

private:
	bool           mShowBoosterHelp[NumBoosters];
	Wind::UIText   mTitle;
	Wind::UIText   mText;
	Wind::UIButton mOKButton;
	Wind::UIBitmap mBoosterIcon;
	Wind::UIBitmap mPanelBitmap;
	Wind::UIPanel  mPanel;
};
