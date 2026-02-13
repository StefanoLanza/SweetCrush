#include "GameOverScreen.h"
#include "Constants.h"
#include "GameDrawOrder.h"
#include "GameUI.h"
#include "Localization.h"
#include "MatchStats.h"
#include "ScreenIds.h"

#include <engine/Engine.h>
#include <engine/UI.h>
#include <engine/UIRenderer.h>

#include <cstdio>

using namespace Wind;

namespace {

constexpr UICanvasDesc canvasDesc {
	.background = "gameartguppy/background.png",
};

const UIPanelDesc panelDesc {
	.pos = UIAbsolutePos(0.f, 0.f),
	.size = UIAbsoluteSize(560.f, 400.f),
	.horizontalAlignment = UIHorizAlignment::center,
	.verticalAlignment = UIVertAlignment::center,
	.background = "UI/button.png",
	.backgroundColor = panel0_color,
	._9patch = 16.f,
};

} // namespace

GameOverScreen::GameOverScreen(MatchStats& matchStats)
    : mMatchStats(matchStats)
    , mTitle { MakeTitleText(GameStringId::gameOver) }
    , mReplayLevelButton { MakeMenuButton(button1_y, GameStringId::retry) }
    , mContinueButton { MakeMenuButton(button2_y, GameStringId::toMainMenu) }
    , mText0 { MakeDynScreenText(text0_y) }
    , mText1 { MakeDynScreenText(text1_y) }
    , mPanel { panelDesc }
    , mCanvas(canvasDesc) {
	mPanel.Add(mText0);
	mPanel.Add(mText1);
	mCanvas.Add(mTitle);
	mCanvas.Add(mPanel);
	mCanvas.Add(mReplayLevelButton);
	mCanvas.Add(mContinueButton);
}

const char* GameOverScreen::GetName() const {
	return "GameOverScreen";
}

void GameOverScreen::LoadAssets(Engine& engine) {
	mCanvas.LoadAssets(engine.GetGraphics(), engine.GetFontManager());
}

ScreenEvent GameOverScreen::Tick(float /*dt*/, const Input& input) {
	mCanvas.HandleInput(input);
	if (mContinueButton.IsClicked()) {
		return GoTo(GameScreenIds::mainMenu, ScreenTransition::slideBottom);
	}
	else if (mReplayLevelButton.IsClicked()) {
		return GoTo(GameScreenIds::play, ScreenTransition::slideBottom);
	}
	return Continue();
}

void GameOverScreen::Draw(UIRenderer& uiRenderer, float dt) {
	mCanvas.Draw(RefWindowWidth, RefWindowHeight, uiRenderer, 0);
}

void GameOverScreen::Enter(const ScreenNavArgs& args) {
	char tmp[256];
	snprintf(tmp, sizeof(tmp), "%s %d", GetLocalizedString(GameStringId::yourReachedLevel), mMatchStats.levelIndex + 1);
	mText0.SetText(tmp);
	snprintf(tmp, sizeof(tmp), "%s %d", GetLocalizedString(GameStringId::yourScoreIs), mMatchStats.score);
	mText1.SetText(tmp);
}

void GameOverScreen::Exit() {
	mMatchStats.levelIndex = 0;
}

void GameOverScreen::ParseConfig(const char* varName, const char* varValue) {
}
