#include "LevelCompletePanel.h"
#include "Localization.h"
#include "MatchStats.h"
#include "ScreenIds.h"
#include "UIDefs.h"
#include <engine/Engine.h>
#include <engine/TextRender.h>
#include <engine/UI.h>

using namespace Wind;

namespace {

const UIButtonDesc buttonDescs[] {
	{
	    UIAbsolutePos(0, 560),
	    UIAutoSize,
	    UIHorizAlignment::center,
	    UIVertAlignment::top,
	},
};

const UITextDesc textDescs[] {
	{
	    "bigFont",
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

} // namespace

LevelCompletePanel::LevelCompletePanel(Engine& engine, const MatchStats& matchStats)
    : mEngine(engine)
    , mMatchStats(matchStats)
    , mTitle(textDescs[0], engine)
    , mNextLevelButton(MakeButton(buttonDescs[0], buttonBitmapDesc, textDescs[1], engine))
    , mPanel(UIDefaultPanelDesc) {
}

void LevelCompletePanel::LoadAssets() {
}

void LevelCompletePanel::BuildUI(UICanvas& canvas) {
	mPanel.AddText(mTitle);
	mPanel.AddButton(mNextLevelButton);
	canvas.GetPanel().AddPanel(mPanel);
}

Wind::GameScreenId LevelCompletePanel::Tick([[maybe_unused]] float dt) {
	const Input& input = mEngine.GetInput();
	if (mNextLevelButton.IsPressed(input)) {
		return ScreenId::play;
	}
	return ScreenId::levelComplete;
}

void LevelCompletePanel::Draw([[maybe_unused]] Wind::GameScreenId topScreen) const {
	// TODO show collected pastries?
}

void LevelCompletePanel::Enter([[maybe_unused]] Wind::GameScreenId prevScreen) {
	mPanel.SetVisible(true);
}

void LevelCompletePanel::Exit() {
	mPanel.SetVisible(false);
}
