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
	.background = "gameartguppy/background.png",
};

} // namespace

PauseScreen::PauseScreen()
    : mTitle { MakeTitleText(GameStringId::pauseGame) }
    , mRestartLevelButton { MakeMenuButton(button0_y, GameStringId::restartLevel) }
    , mEndGameButton { MakeMenuButton(button1_y, GameStringId::endGame) }
    , mSettingsButton { MakeMenuButton(button2_y, GameStringId::settings) }
    , mBackButton { MakeBackButton() }
    , mCanvas(canvasDesc) {
	mCanvas.AddText(mTitle);
	mCanvas.AddButton(mBackButton);
	mCanvas.AddButton(mRestartLevelButton);
	mCanvas.AddButton(mSettingsButton);
	mCanvas.AddButton(mEndGameButton);
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
	if (mEndGameButton.IsClicked(input)) {
		return GoTo(GameScreenIds::mainMenu, ScreenTransition::slideBottom);
	}
	else if (mRestartLevelButton.IsClicked(input)) {
		return GoBack(true, ScreenTransition::slideRight);
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
