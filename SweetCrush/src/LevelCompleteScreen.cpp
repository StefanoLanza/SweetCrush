#include "LevelCompleteScreen.h"
#include "Constants.h"
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

constexpr UICanvasDesc canvasDesc {
	.background = "gameartguppy/background.png",
};

} // namespace

LevelCompleteScreen::LevelCompleteScreen(const MatchStats& matchStats)
    : mMatchStats(matchStats)
    , mTitle(textDescs[0])
    , mNextLevelButton(MakeButton(buttonDescs[0], buttonBitmapDesc, textDescs[1]))
    , mCanvas(canvasDesc) {
	// Setup UI
	mCanvas.AddText(mTitle);
	mCanvas.AddButton(mNextLevelButton);
}

const char* LevelCompleteScreen::GetName() const {
	return "LevelCompleteScreen";
}

void LevelCompleteScreen::LoadAssets(Engine& engine) {
	mCanvas.LoadAssets(engine.GetGraphics(), engine.GetTextRenderer());
}

Wind::ScreenEvent LevelCompleteScreen::Tick([[maybe_unused]] float dt, const Input& input) {
	if (mNextLevelButton.IsPressed(input)) {
		return GoTo(GameScreenIds::play);
	}
	return { ScreenOp::keep };
}

void LevelCompleteScreen::Draw(Wind::UIRenderer& uiRenderer) {
	// TODO show collected pastries?
	mCanvas.Draw(RefWindowWidth, RefWindowHeight, uiRenderer, 0);
}

void LevelCompleteScreen::Enter([[maybe_unused]] Wind::ScreenId prevScreen, const void* payload) {
}

void LevelCompleteScreen::Exit() {
}

void LevelCompleteScreen::ParseConfig(const char* varName, const char* varValue) {
}
