#include "GameCompleteScreen.h"
#include "Constants.h"
#include "GameDrawOrder.h"
#include "GameUI.h"
#include "Localization.h"
#include "MatchStats.h"
#include "ScreenIds.h"

#include <engine/Engine.h>

#include <cstdio>

using namespace Wind;

GameCompleteScreen::GameCompleteScreen(const MatchStats& matchStats)
    : mMatchStats(matchStats)
    , mCanvas(MakeCanvas()) {
	mCanvas.Add(MakeTitle(GameStringId::gameComplete));
	mContinueButton = mCanvas.Add(MakeMenuButton(680.f, GameStringId::continueGame));
	mText0 = mCanvas.Add(MakeScreenText(GameStringId::youCompletedAllLevels, 400));
	mText1 = mCanvas.Add(MakeDynScreenText(460));
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
	if (mAccumTime > 6.f || mContinueButton->IsClicked()) {
		return GoTo(GameScreenIds::mainMenu);
	}
	return Continue();
}

void GameCompleteScreen::Draw(Wind::UIRenderer& uiRenderer, float dt) {
	mCanvas.Draw(RefWindowWidth, RefWindowHeight, uiRenderer, 0);
	char tmp[64];
	snprintf(tmp, sizeof(tmp), "%s %d", GetLocalizedString(GameStringId::yourFinalScoreIs), mMatchStats.score);
	mText1->SetText(tmp);
}

void GameCompleteScreen::Enter(const ScreenNavArgs& args) {
	mAccumTime = 0.f;
}

void GameCompleteScreen::Exit() {
}

void GameCompleteScreen::ParseConfig(const char* varName, const char* varValue) {
}
