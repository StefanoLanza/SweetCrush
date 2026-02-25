#include "LevelCompleteScreen.h"
#include "Constants.h"
#include "GameDataModule.h"
#include "GameUI.h"
#include "Level.h"
#include "Localization.h"
#include "MatchStats.h"
#include "ScreenIds.h"

#include <engine/Engine.h>
#include <engine/TextRender.h>
#include <engine/UI.h>

using namespace Wind;

LevelCompleteScreen::LevelCompleteScreen(MatchStats& matchStats)
    : mMatchStats(matchStats)
    , mNextLevelButton { MakeMenuButton(button2_y, GameStringId::nextLevel) }
    , mCanvas(MakeCanvas()) {
	// Setup UI
	mTitle = mCanvas.Add(MakeTitle(GameStringId::level));
	mCanvas.Add(MakeTitle(GameStringId::complete, subTitleY));
	mCanvas.Add(mNextLevelButton);
}

const char* LevelCompleteScreen::GetName() const {
	return "LevelCompleteScreen";
}

void LevelCompleteScreen::LoadAssets(Engine& engine) {
	mCanvas.LoadAssets(engine.GetGraphics(), engine.GetFontManager());
}

ScreenEvent LevelCompleteScreen::Tick(float dt, const Input& input) {
	mCanvas.Tick(dt);
	mCanvas.HandleInput(input);
	mAccumTime += dt;
	if (mAccumTime > 4.f || mNextLevelButton.IsClicked()) {
		return GoTo(GameScreenIds::levelStart, ScreenTransition::slideTop);
	}
	return Continue();
}

void LevelCompleteScreen::Draw(UIRenderer& uiRenderer, float dt) {
	// TODO show collected pastries?
	mCanvas.Draw(RefWindowWidth, RefWindowHeight, uiRenderer, 0);
}

void LevelCompleteScreen::Enter([[maybe_unused]] const ScreenNavArgs& args) {
	mAccumTime = 0.f;

	char tmp[64];
	snprintf(tmp, sizeof(tmp), "%s %d", GetLocalizedString(GameStringId::level), mMatchStats.levelIndex + 1);
	mTitle->SetText(tmp);
}

void LevelCompleteScreen::Exit() {
	mMatchStats.levelIndex++;
}

void LevelCompleteScreen::ParseConfig(const char* varName, const char* varValue) {
}
