#include "PauseScreen.h"
#include "Constants.h"
#include "Localization.h"
#include "MatchStats.h"
#include "ScreenIds.h"
#include "GameUI.h"

#include <engine/Engine.h>
#include <engine/Input.h>
#include <engine/TextRender.h>
#include <engine/UI.h>

using namespace Wind;

namespace {

const UIButtonDesc buttonDescs[] {
	{
	    .pos = UIAbsolutePos(0, button0_y),
	    .size = UIAutoSize,
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::top,
	},
	{
	    .pos = UIAbsolutePos(0, button1_y),
	    .size = UIAutoSize,
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::top,
	},
	{
	    .pos = UIAbsolutePos(0, button2_y),
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
	{
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::center,
	    .font = "mediumFont",
	    .stringId = GameStringId::settings,
	},
};

constexpr UICanvasDesc canvasDesc {
	.background = "gameartguppy/background.png",
};

} // namespace

PauseScreen::PauseScreen()
    : mTitle(textDescs[0], titleTextStyle)
    , mRestartLevelButton(buttonDescs[0], buttonBitmapDesc, textDescs[1])
    , mExitGameButton(buttonDescs[1], buttonBitmapDesc, textDescs[2])
    , mSettingsButton(buttonDescs[2], buttonBitmapDesc, textDescs[3])
    , mBackButton(defaultBackButtonDesc, defaultBackButtonBitmapDesc)
    , mCanvas(canvasDesc) {
	mCanvas.AddText(mTitle);
	mCanvas.AddButton(mBackButton);
	mCanvas.AddButton(mRestartLevelButton);
	mCanvas.AddButton(mSettingsButton);
	mCanvas.AddButton(mExitGameButton);
}

const char* PauseScreen::GetName() const {
	return "PauseGameScreen";
}

void PauseScreen::LoadAssets(Engine& engine) {
	mCanvas.LoadAssets(engine.GetGraphics(), engine.GetFontManager());
}

ScreenEvent PauseScreen::Tick(float /*dt*/, const Wind::Input& input) {
#if defined(__ANDROID__) || defined(__OHOS__)
	if (input.GetKeyJustPressed(SDLK_AC_BACK)
#elif defined(_WIN32) || defined(__linux__)
	if (input.GetKeyJustPressed(SDLK_ESCAPE)
#endif
	    || mBackButton.IsClicked(input)) {
		return GoBack(false, ScreenTransition::slideRight);
	}
	if (mExitGameButton.IsClicked(input)) {
		return GoTo(GameScreenIds::mainMenu, ScreenTransition::fade);
	}
	else if (mRestartLevelButton.IsClicked(input)) {
		return GoBack(true, ScreenTransition::fade);
	}
	else if (mSettingsButton.IsClicked(input)) {
		return GoTo(GameScreenIds::settings, ScreenTransition::slideLeft);
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
