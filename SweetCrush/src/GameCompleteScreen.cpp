#include "GameCompleteScreen.h"
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

GameCompleteScreen::GameCompleteScreen(const MatchStats& matchStats)
    : mMatchStats(matchStats)
    , mTitle { MakeTitleText(GameStringId::gameComplete) }
    , mContinueButton { MakeMenuButton(680.f, GameStringId::continueGame) }
    , mCanvas(MakeCanvas())
    , mText0 { MakeScreenText(GameStringId::youCompletedAllLevels, 400) }
    , mText1 { MakeDynScreenText(460) } {
	mCanvas.Add(mTitle);
	mCanvas.Add(mContinueButton);
	mCanvas.Add(mText0);
	mCanvas.Add(mText1);
}

const char* GameCompleteScreen::GetName() const {
	return "GameCompleteScreen";
}

void GameCompleteScreen::LoadAssets(Engine& engine) {
	mCanvas.LoadAssets(engine.GetGraphics(), engine.GetFontManager());
}

ScreenEvent GameCompleteScreen::Tick(float dt, const Wind::Input& input) {
	mCanvas.Tick(dt);
	mCanvas.HandleInput(input);
	mAccumTime += dt;
	if (mAccumTime > 4.f || mContinueButton.IsClicked()) {
		return GoTo(GameScreenIds::mainMenu);
	}
	return Continue();
}

void GameCompleteScreen::Draw(Wind::UIRenderer& uiRenderer, float dt) {
	mCanvas.Draw(RefWindowWidth, RefWindowHeight, uiRenderer, 0);
	char tmp[64];
	snprintf(tmp, sizeof(tmp), "%s %d", GetLocalizedString(GameStringId::yourFinalScoreIs), mMatchStats.score);
	mText1.SetText(tmp);
}

void GameCompleteScreen::Enter(const ScreenNavArgs& args) {
	mAccumTime = 0.f;
}

void GameCompleteScreen::Exit() {
}

void GameCompleteScreen::ParseConfig(const char* varName, const char* varValue) {
}
