#include "PauseScreen.h"
#include "Constants.h"
#include "Localization.h"
#include "MatchStats.h"
#include "ScreenIds.h"
#include "UIDefs.h"

#include <engine/Engine.h>
#include <engine/Input.h>
#include <engine/TextRender.h>
#include <engine/UI.h>

using namespace Wind;

namespace {

const UIButtonDesc buttonDescs[] {
	{
	    .pos = UIAbsolutePos(0, 560),
	    .size = UIAutoSize,
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::top,
	},
	{
	    .pos = UIAbsolutePos(0, 680),
	    .size = UIAutoSize,
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::top,
	},
	{
	    .pos = UIAbsolutePos(0, 800),
	    .size = UIAutoSize,
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::top,
	},
};

const UITextDesc textDescs[] {
	{
		.pos = { 0.f, titleY },
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::top,
	    .font = "screenTitle",
	    .stringId = GameStringId::pauseGame,
	    .textStyle = titleTextStyle,
	},
	{
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::center,
	    .font = "mediumFont",
	    .stringId = GameStringId::continueGame,
	},
	{
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::center,
	    .font = "mediumFont",
	    .stringId = GameStringId::restartLevel,
	},
	{
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::center,
	    .font = "mediumFont",
	    .stringId = GameStringId::exitGame,
	},
};

constexpr UICanvasDesc canvasDesc {
	.background = "gameartguppy/background.png",
};

} // namespace

PauseScreen::PauseScreen()
    : mTitle(textDescs[0])
    , mContinueButton(buttonDescs[0], buttonBitmapDesc, textDescs[1])
    , mRestartLevelButton(buttonDescs[1], buttonBitmapDesc, textDescs[2])
    , mExitGameButton(buttonDescs[2], buttonBitmapDesc, textDescs[3])
    , mCanvas(canvasDesc) {
	mCanvas.AddText(mTitle);
	mCanvas.AddButton(mContinueButton);
	mCanvas.AddButton(mRestartLevelButton);
	mCanvas.AddButton(mExitGameButton);
}

const char* PauseScreen::GetName() const {
	return "PauseGameScreen";
}

void PauseScreen::LoadAssets(Engine& engine) {
	mCanvas.LoadAssets(engine.GetGraphics(), engine.GetTextRenderer());
}

ScreenEvent PauseScreen::Tick(float /*dt*/, const Wind::Input& input) {
#if defined(__ANDROID__) || defined(__OHOS__)
	if (input.GetKeyJustPressed(SDLK_AC_BACK)
#elif defined(_WIN32) || defined(__linux__)
	if (input.GetKeyJustPressed(SDLK_ESCAPE)
#endif
	    || mContinueButton.IsClicked(input)) {
		return GoBack(false, ScreenTransition::slideOut);
	}
	if (mExitGameButton.IsClicked(input)) {
		return GoTo(GameScreenIds::mainMenu);
	}
	else if (mRestartLevelButton.IsClicked(input)) {
		return GoBack(true, ScreenTransition::slideOut);
	}
	return Continue();
}

void PauseScreen::Draw(UIRenderer& uiRenderer, float dt) {
	mCanvas.Draw(RefWindowWidth, RefWindowHeight, uiRenderer, 0);
}

void PauseScreen::Enter(const ScreenNavArgs& args) {
}

void PauseScreen::Exit() {
}

void PauseScreen::ParseConfig(const char* varName, const char* varValue) {
}
