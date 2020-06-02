#include "BoostInfoPanel.h"
#include "AssetDefs.h"
#include "Localization.h"
#include "UIDefs.h"

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
	"null.png", UIAbsolutePos(520, 54), UIAbsoluteSize(64, 72), UIHorizAlignment::left, UIVertAlignment::top, whiteColor, UIBlending::on, 1,
};

const UIBitmapDesc panelBitmapDesc {
	"null.png", UIZeroPos, UIParentSize, UIHorizAlignment::center, UIVertAlignment::center, Color{0,0,0,200}, UIBlending::on, 0,
};

const UIPanelDesc panelDesc {
	UIZeroPos,
	UIAbsoluteSize(800, 400),
};

} // namespace

BoostInfoPanel::BoostInfoPanel(Engine& engine)
    : mTitle(textDesc[0], engine)
    , mText(textDesc[1], engine)
    , mOKButton(MakeButton(buttonDesc, shortButtonBitmapDesc, textDesc[2], engine))
    , mBoosterIcon(boosterIconDesc, engine)
    , mPanelBitmap(panelBitmapDesc, engine)
    , mPanel(panelDesc) {
	for (bool& b : mShowBoosterHelp) {
		b = true;
	}
}

void BoostInfoPanel::BuildUI(UICanvas& canvas) {
	mPanel.SetVisible(false);
	mPanel.AddBitmap(mPanelBitmap);
	mPanel.AddBitmap(mBoosterIcon);
	mPanel.AddButton(mOKButton);
	mPanel.AddText(mTitle);
	mPanel.AddText(mText);
	canvas.GetPanel().AddPanel(mPanel);
}

void BoostInfoPanel::ShowHelp(BoosterType boosterType) {
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
		case BoosterType::bomb:
			titleStringId = GameStringId::bomb;
			textStringId = GameStringId::bombDescription;
			break;
		case BoosterType::miniBomb:
			titleStringId = GameStringId::miniBomb;
			textStringId = GameStringId::miniBombDescription;
			break;
		case BoosterType::yellowStar:
		case BoosterType::redStar:
			titleStringId = GameStringId::star;
			textStringId = GameStringId::starDescription;
			break;
	}
	mTitle.SetText(static_cast<StringId>(titleStringId));
	mText.SetText(static_cast<StringId>(textStringId));
	mBoosterIcon.SetBitmap(sprites[boosterDefs[typeIdx].sprite]);
	mPanel.SetVisible(true);
}

bool BoostInfoPanel::IsVisible() const {
	return mPanel.IsVisible();
}

bool BoostInfoPanel::Wait(const Input& input) {
	if (mPanel.IsVisible()) {
		if (mOKButton.IsPressed(input)) {
			Hide();
			return false;
		}
		return true;
	}
	return false;
}

void BoostInfoPanel::Hide() {
	mPanel.SetVisible(false);
}
