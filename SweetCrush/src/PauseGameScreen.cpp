#include "PauseGameScreen.h"
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
	    UIAbsolutePos(0, 440),
	    UIAutoSize,
	    UIHorizAlignment::center,
	    UIVertAlignment::top,
	},
	{
	    UIAbsolutePos(0, 560),
	    UIAutoSize,
	    UIHorizAlignment::center,
	    UIVertAlignment::top,
	},
	{
	    UIAbsolutePos(0, 680),
	    UIAutoSize,
	    UIHorizAlignment::center,
	    UIVertAlignment::top,
	},
};

const UITextDesc textDescs[] {
	{
	    "bigFont",
	    (StringId)GameStringId::pauseGame,
	    UIAbsolutePos(0, titleY),
	    UIAutoSize,
	    UIHorizAlignment::center,
	    UIVertAlignment::top,
	    titleTextStyle,
	},
	{
	    "mediumFont",
	    (StringId)GameStringId::continueGame,
	    UIZeroPos,
	    UIAutoSize,
	    UIHorizAlignment::center,
	    UIVertAlignment::center,
	    defaultTextStyle,
	},
	{
	    "mediumFont",
	    (StringId)GameStringId::restartLevel,
	    UIZeroPos,
	    UIAutoSize,
	    UIHorizAlignment::center,
	    UIVertAlignment::center,
	    defaultTextStyle,
	},
	{
	    "mediumFont",
	    (StringId)GameStringId::exitGame,
	    UIZeroPos,
	    UIAutoSize,
	    UIHorizAlignment::center,
	    UIVertAlignment::center,
	    defaultTextStyle,
	},
};

} // namespace

PauseGameScreen::PauseGameScreen(Engine& engine)
    : mEngine(engine)
    , mTitle(textDescs[0], engine)
    , mContinueButton(MakeButton(buttonDescs[0], buttonBitmapDesc, textDescs[1], engine))
    , mRestartLevelButton(MakeButton(buttonDescs[1], buttonBitmapDesc, textDescs[2], engine))
    , mExitGameButton(MakeButton(buttonDescs[2], buttonBitmapDesc, textDescs[3], engine))
    , mPanel(UIDefaultPanelDesc) {
}

const char* PauseGameScreen::GetName() const {
	return "PauseGameScreen";
}

void PauseGameScreen::LoadAssets() {
}

void PauseGameScreen::BuildUI(UICanvas& canvas) {
	mPanel.AddText(mTitle);
	mPanel.AddButton(mContinueButton);
	mPanel.AddButton(mRestartLevelButton);
	mPanel.AddButton(mExitGameButton);
	canvas.GetPanel().AddPanel(mPanel);
}

Wind::GameScreenId PauseGameScreen::Tick(float /*dt*/, const Wind::Input& input) {
#if defined(__ANDROID__) || defined(__OHOS__)
	if (input.GetKeyPressed(SDLK_AC_BACK)) {
#elif defined(_WIN32) || defined(__linux__)
	if (input.GetKeyPressed(SDLK_ESCAPE)) {
#endif
		return ScreenId::play;
	}
	if (mExitGameButton.IsPressed(input)) {
		return ScreenId::mainMenu;
	}
	else if (mRestartLevelButton.IsPressed(input)) {
		//TODO mRestartLevel = true; // FIXME Return as generic data
		return ScreenId::play;
	}
	else if (mContinueButton.IsPressed(input)) {
		//TODO mRestartLevel = false;
		return ScreenId::play;
	}
	return ScreenId::pauseGame;
}

void PauseGameScreen::Draw([[maybe_unused]] GameScreenId topScreen) const {
}

void PauseGameScreen::Enter([[maybe_unused]] GameScreenId prevScreen, const void* payload) {
	mPanel.SetVisible(true);
}

void PauseGameScreen::Exit() {
	mPanel.SetVisible(false);
}

void PauseGameScreen::ParseConfig(const char* varName, const char* varValue) {
}
