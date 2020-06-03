#include "LevelCompletePanel.h"
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
	    UIAbsolutePos(0, 440),
	    UIAutoSize,
	    UIHorizAlignment::center,
	    UIVertAlignment::top,
	},
};

const UITextDesc textDescs[2] {
	{
	    "mediumFont",
	    (StringId)GameStringId::levelComplete,
	    UIAbsolutePos(0, titleY),
	    UIAutoSize,
	    UIHorizAlignment::center,
	    UIVertAlignment::top,
	    titleTextStyle,
	},
	{
	    "mediumFont",
	    (StringId)GameStringId::nextLevel,
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
	UIAbsoluteSize(800, 420),
};

} // namespace

LevelCompletePanel::LevelCompletePanel(Engine& engine)
    : mEngine(engine)
    , mTitle(textDescs[0], engine)
    , mNextLevelButton(MakeButton(buttonDescs[0], buttonBitmapDesc, textDescs[1], engine))
    , mPanelBitmap(panelBitmapDesc, engine)
    , mPanel(UIDefaultPanelDesc) {
}

void LevelCompletePanel::LoadAssets() {
}

void LevelCompletePanel::BuildUI(UICanvas& canvas) {
	mPanel.AddText(mTitle);
	mPanel.AddButton(mNextLevelButton);
	mPanel.AddBitmap(mPanelBitmap);
	canvas.GetPanel().AddPanel(mPanel);
}

Wind::GameScreenId LevelCompletePanel::Tick(float dt) {
	const Input& input = mEngine.GetInput();
	if (mNextLevelButton.IsPressed(input)) {
		return ScreenId::play;
	}
	return ScreenId::levelComplete;
}

void LevelCompletePanel::Draw(Wind::GameScreenId topScreen) const {
}

void LevelCompletePanel::Enter(Wind::GameScreenId prevScreen) {
	mPanel.SetVisible(true);
}

void LevelCompletePanel::Exit() {
	mPanel.SetVisible(false);
}
