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

const UITextDesc textDescs[5] {
	{ "bigFont", (StringId)GameStringId::credits, { 0, titleY, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top, titleTextStyle },
	{ "smallFont", (StringId)GameStringId::codeBy, { 0, 420, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top, defaultTextStyle },
	{ "smallFont",
	  (StringId)GameStringId::graphicsBy,
	  { 0, 480, 0, 0 },
	  UIAutoSize,
	  UIHorizAlignment::center,
	  UIVertAlignment::top,
	  defaultTextStyle },
	{ "smallFont", (StringId)GameStringId::audioBy, { 0, 540, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top, defaultTextStyle },
	{ "mediumFont", (StringId)GameStringId::back, UIZeroPos, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::center, defaultTextStyle },
};

} // namespace

CreditsScreen::CreditsScreen(Engine& engine)
    : mEngine(engine)
    , mTitle(textDescs[0], engine)
    , mCodeBy(textDescs[1], engine)
    , mGraphicsBy(textDescs[2], engine)
    , mMusicBy(textDescs[3], engine)
    , mBackButton(MakeButton(buttonDescs[0], buttonBitmapDesc, textDescs[4], engine))
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
	mPanel.AddButton(mBackButton);
	canvas.GetPanel().AddPanel(mPanel);
}