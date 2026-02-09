#include "PauseScreen.h"
#include "Constants.h"
#include "GameUI.h"
#include "Localization.h"
#include "MatchStats.h"
#include "ScreenIds.h"

#include <engine/Engine.h>
#include <engine/Input.h>
#include <engine/TextRender.h>
#include <engine/UI.h>

using namespace Wind;

namespace {

constexpr UICanvasDesc canvasDesc {
	.backgroundColor = { 127.5f, 127.5f, 127.5f, 27.5f },
};

constexpr UIPanelDesc panelDesc {
	.pos = UIZeroPos,
	.size = UIAbsoluteSize(600.f, 500.f),
	.horizontalAlignment = UIHorizAlignment::center,
	.verticalAlignment = UIVertAlignment::center,
	.background = "UI/blueSquareRect.png",
	.backgroundColor = whiteColor,
	._9patch = 16.f,
};

} // namespace

PauseScreen::PauseScreen()
    : mTitle { MakeTitleText(GameStringId::pauseGame) }
    , mRestartLevelButton { MakeMenuButton(100, GameStringId::restartLevel) }
    , mEndGameButton { MakeMenuButton(220, GameStringId::endGame) }
    , mSettingsButton { MakeMenuButton(340, GameStringId::settings) }
    , mCloseButton { MakeCloseButton() }
    , mPanel { panelDesc }
    , mCanvas(canvasDesc) {
	// mCanvas.AddText(mTitle);
	mCanvas.AddPanel(mPanel);
	mPanel.AddButton(mCloseButton);
	mPanel.AddButton(mRestartLevelButton);
	mPanel.AddButton(mSettingsButton);
	mPanel.AddButton(mEndGameButton);
}

const char* PauseScreen::GetName() const {
	return "PauseGameScreen";
}

void PauseScreen::LoadAssets(Engine& engine) {
	mCanvas.LoadAssets(engine.GetGraphics(), engine.GetFontManager());
}

ScreenEvent PauseScreen::Tick(float /*dt*/, const Wind::Input& input) {
	mCanvas.HandleInput(input);
#if defined(__ANDROID__) || defined(__OHOS__)
	if (input.GetKeyJustPressed(SDLK_AC_BACK)
#elif defined(_WIN32) || defined(__linux__)
	if (input.GetKeyJustPressed(SDLK_ESCAPE)
#endif
	    || mCloseButton.IsClicked()) {
		return GoBack(false);
	}
	if (mEndGameButton.IsClicked()) {
		return GoTo(GameScreenIds::mainMenu);
	}
	else if (mRestartLevelButton.IsClicked()) {
		return GoBack(true);
	}
	else if (mSettingsButton.IsClicked()) {
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
