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
	    .pos = UIAbsolutePos(0, 440),
	    .size = UIAutoSize,
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::top,
	},
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
};

const UITextDesc textDescs[] {
	{
	    .pos = UIAbsolutePos(0, titleY),
	    .size = UIAutoSize,
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::top,
	    .font = "bigFont",
	    .stringId = (StringId)GameStringId::pauseGame,
	    .textStyle = titleTextStyle,
	},
	{
	    .pos = UIZeroPos,
	    .size = UIAutoSize,
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::center,
	    .font = "mediumFont",
	    .stringId = (StringId)GameStringId::continueGame,
	},
	{
	    .pos = UIZeroPos,
	    .size = UIAutoSize,
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::center,
	    .font = "mediumFont",
	    .stringId = (StringId)GameStringId::restartLevel,
	},
	{
	    .pos = UIZeroPos,
	    .size = UIAutoSize,
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::center,
	    .font = "mediumFont",
	    .stringId = (StringId)GameStringId::exitGame,
	},
};

constexpr UICanvasDesc canvasDesc {
	.background = "gameartguppy/background.png",
};

} // namespace

PauseScreen::PauseScreen(Engine& engine)
    : mEngine(engine)
    , mTitle(textDescs[0], engine.GetTextRenderer())
    , mContinueButton(MakeButton(buttonDescs[0], buttonBitmapDesc, textDescs[1], engine))
    , mRestartLevelButton(MakeButton(buttonDescs[1], buttonBitmapDesc, textDescs[2], engine))
    , mExitGameButton(MakeButton(buttonDescs[2], buttonBitmapDesc, textDescs[3], engine))
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
	mCanvas.LoadGraphics(engine.GetGraphics());
}

ScreenTransition PauseScreen::Tick(float /*dt*/, const Wind::Input& input) {
#if defined(__ANDROID__) || defined(__OHOS__)
	if (input.GetKeyJustPressed(SDLK_AC_BACK)) {
#elif defined(_WIN32) || defined(__linux__)
	if (input.GetKeyJustPressed(SDLK_ESCAPE)) {
#endif
		return { ScreenOp::pop };
	}
	if (mExitGameButton.IsPressed(input)) {
		return { ScreenOp::replace, GameScreenIds::mainMenu };
	}
	else if (mRestartLevelButton.IsPressed(input)) {
		// TODO mRestartLevel = true; // FIXME Return as generic data
		return { ScreenOp::pop };
	}
	else if (mContinueButton.IsPressed(input)) {
		// TODO mRestartLevel = false;
		return { ScreenOp::pop };
	}
	return { ScreenOp::keep };
}

void PauseScreen::Draw(Wind::UIRenderer& uiRenderer) {
	mCanvas.Draw(RefWindowWidth, RefWindowHeight, uiRenderer, 0);
}

void PauseScreen::Enter([[maybe_unused]] ScreenId prevScreen, const void* payload) {
}

void PauseScreen::Exit() {
}

void PauseScreen::ParseConfig(const char* varName, const char* varValue) {
}
