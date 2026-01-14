#include "EffectInfoPanel.h"
#include "AssetDefs.h"
#include "Localization.h"
#include "UIDefs.h"

#include <engine/Engine.h>

using namespace Wind;

namespace {

const UIButtonDesc buttonDesc {
	UIAbsolutePos(0, 260),
	UIAutoSize,
	UIHorizAlignment::center,
	UIVertAlignment::top,
};

const UITextDesc textDesc[] {
	{
	    "mediumFont",
	    (StringId)GameStringId::empty,
	    UIAbsolutePos(0, 60),
	    UIAutoSize,
	    UIHorizAlignment::center,
	    UIVertAlignment::top,
	    defaultTextStyle,
	},
	{
	    "smallFont",
	    (StringId)GameStringId::empty,
	    UIAbsolutePos(0, 160),
	    UIAutoSize,
	    UIHorizAlignment::center,
	    UIVertAlignment::top,
	    defaultTextStyle,
	},
	{
	    "mediumFont",
	    (StringId)GameStringId::ok,
	    UIZeroPos,
	    UIAutoSize,
	    UIHorizAlignment::center,
	    UIVertAlignment::center,
	    defaultTextStyle,
	},
};

const UIBitmapDesc boosterIconDesc {
	"null.png", UIAbsolutePos(540, 50), UIAbsoluteSize(64, 72), UIHorizAlignment::left, UIVertAlignment::top, whiteColor, UIBlending::on, 1,
};

const UIBitmapDesc panelBitmapDesc {
	"null.png", UIZeroPos, UIParentSize, UIHorizAlignment::center, UIVertAlignment::center, Color { 0, 0, 0, 200 }, UIBlending::on, 0,
};

const UIPanelDesc panelDesc {
	UIZeroPos,
	UIAbsoluteSize(800, 400),
};

} // namespace

EffectInfoPanel::EffectInfoPanel(Engine& engine)
    : mTitle(textDesc[0], engine)
    , mText(textDesc[1], engine)
    , mOKButton(MakeButton(buttonDesc, buttonBitmapDesc, textDesc[2], engine))
    , mBoosterIcon(boosterIconDesc, engine.GetGraphics())
    , mPanelBitmap(panelBitmapDesc, engine.GetGraphics())
    , mPanel(panelDesc) {
	for (bool& b : mShowBoosterHelp) {
		b = true;
	}
}

void EffectInfoPanel::BuildUI(UICanvas& canvas) {
	mPanel.SetVisible(false);
	mPanel.AddBitmap(mPanelBitmap);
	mPanel.AddBitmap(mBoosterIcon);
	mPanel.AddButton(mOKButton);
	mPanel.AddText(mTitle);
	mPanel.AddText(mText);
	canvas.GetPanel().AddPanel(mPanel);
}

void EffectInfoPanel::ShowHelp(EffectType boosterType) {
	const int typeIdx = static_cast<int>(boosterType);
	if (! mShowBoosterHelp[typeIdx]) {
		return;
	}
	if (mShowBoosterHelp[typeIdx]) {
		mShowBoosterHelp[typeIdx] = false;
	}
	GameStringId titleStringId = GameStringId::empty;
	GameStringId textStringId = GameStringId::empty;
	switch (boosterType) {
	case EffectType::hrocket:
		titleStringId = GameStringId::hRocket;
		textStringId = GameStringId::hRocketDescription;
		break;
	case EffectType::vrocket:
		titleStringId = GameStringId::vRocket;
		textStringId = GameStringId::vRocketDescription;
		break;
	case EffectType::bomb:
		titleStringId = GameStringId::bomb;
		textStringId = GameStringId::bombDescription;
		break;
	case EffectType::miniBomb:
		titleStringId = GameStringId::miniBomb;
		textStringId = GameStringId::miniBombDescription;
		break;
	default:
		break;
	}
	mTitle.SetText(static_cast<StringId>(titleStringId));
	mText.SetText(static_cast<StringId>(textStringId));
	mBoosterIcon.SetBitmap(sprites[boosterDefs[typeIdx].sprite]);
	mPanel.SetVisible(true);
}

bool EffectInfoPanel::IsVisible() const {
	return mPanel.IsVisible();
}

bool EffectInfoPanel::Wait(const Input& input) {
	if (mPanel.IsVisible()) {
		if (mOKButton.IsPressed(input)) {
			Hide();
			return false;
		}
		return true;
	}
	return false;
}

void EffectInfoPanel::Hide() {
	mPanel.SetVisible(false);
}
