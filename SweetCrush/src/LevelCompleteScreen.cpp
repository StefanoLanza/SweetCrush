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
	    UIAbsolutePos(0, button2_y),
	    UIAutoSize,
	    UIHorizAlignment::center,
	    UIVertAlignment::top,
	},
};

const UITextDesc textDescs[] {
	{
		.pos = { 0.f, titleY },
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::top,

	    .font = "screenTitle",
	    .stringId = GameStringId::levelComplete,
	    .textStyle = titleTextStyle,
	},
	{
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::center,
	    .font = "mediumFont",
	    .stringId = GameStringId::nextLevel,
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
    , mNextLevelButton(buttonDescs[0], buttonBitmapDesc, textDescs[1])
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

ScreenEvent LevelCompleteScreen::Tick(float dt, const Input& input) {
	mAccumTime += dt;
	if (mAccumTime > 4.f || mNextLevelButton.IsClicked(input)) {
		return GoTo(GameScreenIds::levelStart);
	}
	return Continue();
}

void LevelCompleteScreen::Draw(UIRenderer& uiRenderer, float dt) {
	// TODO show collected pastries?
	mCanvas.Draw(RefWindowWidth, RefWindowHeight, uiRenderer, 0);
}

void LevelCompleteScreen::Enter([[maybe_unused]] const ScreenNavArgs& args) {
	mAccumTime = 0.f;
}

void LevelCompleteScreen::Exit() {
}

void LevelCompleteScreen::ParseConfig(const char* varName, const char* varValue) {
}
