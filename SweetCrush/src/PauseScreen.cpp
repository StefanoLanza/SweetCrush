#include "PauseScreen.h"
#include "Constants.h"
#include "GameUI.h"
#include "Localization.h"
#include "MatchStats.h"
#include "ScreenIds.h"

#include <engine/Engine.h>
#include <engine/Input.h>

using namespace Wind;

namespace {

constexpr UICanvasDesc canvasDesc {
	.backgroundColor = { 27.5f, 27.5f, 27.5f, 200.5f },
};

const UIPanelDesc panelDesc {
	.pos = UIZeroPos,
	.size = UIAbsoluteSize(600.f, 500.f),
	.horizontalAlignment = UIHorizAlignment::center,
	.verticalAlignment = UIVertAlignment::center,
	.background = "UI/panel.png",
	.backgroundColor = panel0_color,
	._9patch = 16.f,
};

} // namespace

PauseScreen::PauseScreen()
    : mRestartLevelButton { MakeMenuButton(80, GameStringId::restartLevel, button0_color) }
    , mEndGameButton { MakeMenuButton(200, GameStringId::endGame, button2_color) }
    , mSettingsButton { MakeMenuButton(440, GameStringId::settings, button1_color) }
    , mCloseButton { MakeMenuButton(320, GameStringId::continueGame, button3_color) }
//    , mCloseButton { MakeCloseButton() }
    , mPanel { panelDesc }
    , mCanvas(canvasDesc) {
	mCanvas.Add(mPanel);
	mPanel.Add(mCloseButton);
	mPanel.Add(mRestartLevelButton);
	// mPanel.Add(mSettingsButton.GetPanel());
	mPanel.Add(mEndGameButton);
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
		return GoTo(GameScreenIds::mainMenu, ScreenTransition::slideBottom);
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
