#include "LeaveGamePanel.h"
#include "Localization.h"
#include "ScreenIds.h"
#include "UIDefs.h"
#include <engine/Engine.h>
#include <engine/TextRender.h>
#include <engine/UI.h>

using namespace Wind;

namespace {

const UIButtonDesc buttonDescs[2] {
	{
	    UIAbsolutePos(-140, -100),
	    UIAutoSize,
	    UIHorizAlignment::center,
	    UIVertAlignment::bottom,
	},
	{
	    UIAbsolutePos(+140, -100),
	    UIAutoSize,
	    UIHorizAlignment::center,
	    UIVertAlignment::bottom,
	},
};

const UITextDesc textDescs[3] {
	{
	    "mediumFont",
	    (StringId)GameStringId::leave,
	    UIAbsolutePos(0, 100),
	    UIAutoSize,
	    UIHorizAlignment::center,
	    UIVertAlignment::top,
	    titleTextStyle,
	},
	{
	    "mediumFont",
	    (StringId)GameStringId::yes,
	    UIZeroPos,
	    UIAutoSize,
	    UIHorizAlignment::center,
	    UIVertAlignment::center,
	    defaultTextStyle,
	},
	{
	    "mediumFont",
	    (StringId)GameStringId::no,
	    UIZeroPos,
	    UIAutoSize,
	    UIHorizAlignment::center,
	    UIVertAlignment::center,
	    defaultTextStyle,
	},
};

const UIBitmapDesc panelBitmapDesc {
	"null.png", UIZeroPos, UIParentSize, UIHorizAlignment::center, UIVertAlignment::center, Color { 0, 0, 0, 200 }, UIBlending::on, 0,
};

const UIPanelDesc panelDesc {
	UIZeroPos,
	UIAbsoluteSize(800, 400),
};

} // namespace

LeaveGamePanel::LeaveGamePanel(Engine& engine)
    : mEngine(engine)
    , mTitle(textDescs[0], engine)
    , mYesButton(MakeButton(buttonDescs[0], shortButtonBitmapDesc, textDescs[1], engine))
    , mNoButton(MakeButton(buttonDescs[1], shortButtonBitmapDesc, textDescs[2], engine))
    , mPanelBitmap(panelBitmapDesc, engine)
    , mPanel(panelDesc) {
}

void LeaveGamePanel::LoadAssets() {
}

void LeaveGamePanel::BuildUI(UICanvas& canvas) {
	mPanel.AddText(mTitle);
	mPanel.AddButton(mYesButton);
	mPanel.AddButton(mNoButton);
	mPanel.AddBitmap(mPanelBitmap);
	canvas.GetPanel().AddPanel(mPanel);
}

Wind::GameScreenId LeaveGamePanel::Tick(float /*dt*/) {
	const Input& input = mEngine.GetInput();
	if (mYesButton.IsPressed(input)) {
		return ScreenId::mainMenu;
	}
	else if (mNoButton.IsPressed(input)) {
		return ScreenId::play;
	}
	return ScreenId::leaveGame;
}

void LeaveGamePanel::Draw(GameScreenId topScreen) const {
}

void LeaveGamePanel::Enter(GameScreenId prevScreen) {
	mPanel.SetVisible(true);
}

void LeaveGamePanel::Exit() {
	mPanel.SetVisible(false);
}
