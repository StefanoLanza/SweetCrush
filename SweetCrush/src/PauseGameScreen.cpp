#include "PauseGameScreen.h"
#include "Localization.h"
#include "ScreenIds.h"
#include "UIDefs.h"
#include <engine/Engine.h>
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

void PauseGameScreen::LoadAssets() {
}

void PauseGameScreen::BuildUI(UICanvas& canvas) {
	mPanel.AddText(mTitle);
	mPanel.AddButton(mContinueButton);
	mPanel.AddButton(mRestartLevelButton);
	mPanel.AddButton(mExitGameButton);
	canvas.GetPanel().AddPanel(mPanel);
}

Wind::GameScreenId PauseGameScreen::Tick(float /*dt*/) {
	const Input& input = mEngine.GetInput();
	if (mExitGameButton.IsPressed(input)) {
		return ScreenId::mainMenu;
	}
	else if (mContinueButton.IsPressed(input)) {
		return ScreenId::play;
	}
	return ScreenId::pauseGame;
}

void PauseGameScreen::Draw(GameScreenId topScreen) const {
}

void PauseGameScreen::Enter(GameScreenId prevScreen) {
	mPanel.SetVisible(true);
}

void PauseGameScreen::Exit() {
	mPanel.SetVisible(false);
}
