#include "LevelCompleteScreen.h"
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
	    .pos = UIAbsolutePos(0, titleY),
	    .size = UIAutoSize,
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::top,

	    .font = "bigFont",
	    .stringId = (StringId)GameStringId::levelComplete,
	    .textStyle = titleTextStyle,
	},
	{
	    .pos = UIZeroPos,
	    .size = UIAutoSize,
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::center,
	    .font = "mediumFont",
	    .stringId = (StringId)GameStringId::nextLevel,
	    .textStyle = defaultTextStyle,
	},
};

} // namespace

LevelCompleteScreen::LevelCompleteScreen(Engine& engine, const MatchStats& matchStats)
    : mEngine(engine)
    , mMatchStats(matchStats)
    , mTitle(textDescs[0], engine)
    , mNextLevelButton(MakeButton(buttonDescs[0], buttonBitmapDesc, textDescs[1], engine))
    , mPanel(UIDefaultPanelDesc) {
}

const char* LevelCompleteScreen::GetName() const {
	return "LevelCompleteScreen";
}

void LevelCompleteScreen::LoadAssets(Engine& engine) {
}

void LevelCompleteScreen::BuildUI(UICanvas& canvas) {
	mPanel.AddText(mTitle);
	mPanel.AddButton(mNextLevelButton);
	canvas.GetPanel().AddPanel(mPanel);
}

Wind::ScreenTransition LevelCompleteScreen::Tick([[maybe_unused]] float dt, const Wind::Input& input) {
	if (mNextLevelButton.IsPressed(input)) {
		return { ScreenOp::replace, GameScreenIds::play };
	}
	return { ScreenOp::keep };
}

void LevelCompleteScreen::Draw(Wind::UIRenderer& uiRenderer) {
	// TODO show collected pastries?
}

void LevelCompleteScreen::Enter([[maybe_unused]] Wind::ScreenId prevScreen, const void* payload) {
	mPanel.SetVisible(true);
}

void LevelCompleteScreen::Exit() {
	mPanel.SetVisible(false);
}

void LevelCompleteScreen::ParseConfig(const char* varName, const char* varValue) {
}
