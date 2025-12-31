#include "CreditsScreen.h"
#include "Localization.h"
#include "ScreenIds.h"
#include "UIDefs.h"
#include <engine/Engine.h>
#include <engine/TextRender.h>
#include <engine/UI.h>

using namespace Wind;

namespace {

const UIButtonDesc buttonDescs[1] {
	{
	    UIAbsolutePos(0, 680),
	    UIAutoSize,
	    UIHorizAlignment::center,
	    UIVertAlignment::top,
	},
};

const UITextDesc titleText {
	"bigFont", (StringId)GameStringId::credits, { 0, titleY, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top, titleTextStyle,
};

const UITextDesc codeByText {
	"smallFont", (StringId)GameStringId::codeBy, { 0, 360, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top, defaultTextStyle,
};

const UITextDesc graphicsByText {
	"smallFont", (StringId)GameStringId::graphicsBy, { 0, 420, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top, defaultTextStyle,
};

const UITextDesc musicByText {
	"smallFont", (StringId)GameStringId::musicBy, { 0, 480, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top, defaultTextStyle,
};

const UITextDesc versionText {
	"smallFont", (StringId)GameStringId::version, { 0, 540, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top, defaultTextStyle,
};

const UITextDesc backText {
	"mediumFont", (StringId)GameStringId::back, UIZeroPos, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::center, defaultTextStyle,
};

} // namespace

CreditsScreen::CreditsScreen(Engine& engine)
    : mTitle(titleText, engine)
    , mCodeBy(codeByText, engine)
    , mGraphicsBy(graphicsByText, engine)
    , mMusicBy(musicByText, engine)
    , mVersion(versionText, engine)
    , mBackButton(MakeButton(buttonDescs[0], buttonBitmapDesc, backText, engine))
    , mPanel(UIDefaultPanelDesc) {
}

GameScreenId CreditsScreen::Tick(float /*dt*/, const Wind::Input& input) {
	if (mBackButton.IsPressed(input)) {
		return ScreenId::mainMenu;
	}
	return ScreenId::credits;
}

void CreditsScreen::Draw([[maybe_unused]] GameScreenId topScreen) const {
}

void CreditsScreen::Enter([[maybe_unused]] GameScreenId prevScreen) {
	mPanel.SetVisible(true);
}

void CreditsScreen::Exit() {
	mPanel.SetVisible(false);
}

void CreditsScreen::LoadAssets() {
}

void CreditsScreen::BuildUI(UICanvas& canvas) {
	mPanel.AddText(mTitle);
	mPanel.AddText(mCodeBy);
	mPanel.AddText(mGraphicsBy);
	mPanel.AddText(mMusicBy);
	mPanel.AddText(mVersion);
	mPanel.AddButton(mBackButton);
	canvas.GetPanel().AddPanel(mPanel);
}