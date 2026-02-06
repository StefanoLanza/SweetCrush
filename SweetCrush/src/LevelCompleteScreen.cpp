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

namespace {

constexpr UICanvasDesc canvasDesc {
	.background = "gameartguppy/background.png",
};

} // namespace

LevelCompleteScreen::LevelCompleteScreen(MatchStats& matchStats, const GameDataModule& gameDataModule)
    : mMatchStats(matchStats)
    , mGameDataModule { gameDataModule }
    , mTitle { MakeTitleText(GameStringId::level) }
    , mSubTitle { MakeSubTitleText(GameStringId::complete) }
    , mNextLevelButton { MakeMenuButton(button2_y, GameStringId::nextLevel) }
    , mCanvas(canvasDesc) {
	// Setup UI
	mCanvas.AddText(mTitle);
	mCanvas.AddText(mSubTitle);
	mCanvas.AddButton(mNextLevelButton);
}

const char* LevelCompleteScreen::GetName() const {
	return "LevelCompleteScreen";
}

void LevelCompleteScreen::LoadAssets(Engine& engine) {
	mCanvas.LoadAssets(engine.GetGraphics(), engine.GetFontManager());
}

ScreenEvent LevelCompleteScreen::Tick(float dt, const Input& input) {
	mAccumTime += dt;
	if (mAccumTime > 4.f || mNextLevelButton.IsClicked(input)) {
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

	char         tmp[256];
	const Level& level = *mGameDataModule.GetLevel(mMatchStats.levelIndex);
	snprintf(tmp, sizeof(tmp), "%s %d", GetLocalizedString(GameStringId::level), mMatchStats.levelIndex + 1);
	mTitle.SetText(tmp);
}

void LevelCompleteScreen::Exit() {
	mMatchStats.levelIndex++;
}

void LevelCompleteScreen::ParseConfig(const char* varName, const char* varValue) {
}
