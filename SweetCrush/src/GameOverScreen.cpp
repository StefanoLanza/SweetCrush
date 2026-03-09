#include "GameOverScreen.h"
#include "Constants.h"
#include "GameUI.h"
#include "Localization.h"
#include "MatchStats.h"
#include "ScreenIds.h"

#include <engine/Engine.h>

#include <cstdio>

using namespace Wind;

namespace {

const UIPanelDesc panelDesc {
	.pos = UIAbsolutePos(0.f, 0.f),
	.size = UIAbsoluteSize(520.f, 300.f),
	.horizontalAlignment = UIHorizAlignment::center,
	.verticalAlignment = UIVertAlignment::center,
	.background = "UI/button.png",
	.backgroundColor = panel0_color,
	._9patch = 16.f,
};

}

GameOverScreen::GameOverScreen(MatchStats& matchStats)
    : mMatchStats(matchStats)
    , mCanvas(MakeCanvas()) {
	// Build UI
	mCanvas.Add(MakeTitle(GameStringId::gameOver));
	auto panel = mCanvas.Add(panelDesc);
	mText0 = panel->Add(MakeDynScreenText(40.f));
	mText1 = panel->Add(MakeDynScreenText(100.f));
	mReplayLevelButton = MakeMenuButton(mCanvas.Panel(), button2_y, GameStringId::retry, button0_color, "icons/replay.png");
	mEndButton = MakeMenuButton(mCanvas.Panel(),button3_y, GameStringId::toMainMenu, button1_color, "icons/cross.png");
}

const char* GameOverScreen::GetName() const {
	return "GameOverScreen";
}

void GameOverScreen::LoadAssets(Engine& engine) {
	mCanvas.LoadAssets(engine.GetGraphics(), engine.GetFontManager());
}

ScreenEvent GameOverScreen::Tick(float dt, const Input& input) {
	mCanvas.Tick(dt);
	mCanvas.HandleInput(input);
	if (mEndButton->IsClicked()) {
		return GoTo(GameScreenIds::mainMenu, ScreenTransition::slideBottom);
	}
	else if (mReplayLevelButton->IsClicked()) {
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
	mText0->SetText(tmp);
	snprintf(tmp, sizeof(tmp), "%s %d", GetLocalizedString(GameStringId::yourScoreIs), mMatchStats.score);
	mText1->SetText(tmp);
}

void GameOverScreen::Exit() {
	mMatchStats.levelIndex = 0;
}

void GameOverScreen::ParseConfig(const char* varName, const char* varValue) {
}
