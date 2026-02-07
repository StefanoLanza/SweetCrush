#include "GameCompleteScreen.h"
#include "Constants.h"
#include "GameDrawOrder.h"
#include "GameUI.h"
#include "Localization.h"
#include "MatchStats.h"
#include "ScreenIds.h"

#include <engine/Engine.h>
#include <engine/FontManager.h>
#include <engine/TextRender.h>
#include <engine/UI.h>
#include <engine/UIRenderer.h>

#include <cstdio>

using namespace Wind;

namespace {

constexpr UICanvasDesc canvasDesc {
	.background = "gameartguppy/background.png",
};

} // namespace

GameCompleteScreen::GameCompleteScreen(const MatchStats& matchStats)
    : mMatchStats(matchStats)
    , mTitle { MakeTitleText(GameStringId::gameComplete) }
    , mContinueButton { MakeMenuButton(680.f, GameStringId::continueGame) }
    , mCanvas(canvasDesc) {
	mCanvas.AddText(mTitle);
	mCanvas.AddButton(mContinueButton);
}

const char* GameCompleteScreen::GetName() const {
	return "GameCompleteScreen";
}

void GameCompleteScreen::LoadAssets(Engine& engine) {
	mCanvas.LoadAssets(engine.GetGraphics(), engine.GetFontManager());
	mFont = engine.GetFontManager().AddFont("smallFont");
}

ScreenEvent GameCompleteScreen::Tick(float dt, const Wind::Input& input) {
	mCanvas.HandleInput(input);
	mAccumTime += dt;
	if (mAccumTime > 4.f || mContinueButton.IsClicked()) {
		return GoTo(GameScreenIds::mainMenu);
	}
	return Continue();
}

void GameCompleteScreen::Draw(Wind::UIRenderer& uiRenderer, float dt) {
	mCanvas.Draw(RefWindowWidth, RefWindowHeight, uiRenderer, 0);
	if (! mFont) {
		return;
	}
	const auto&     textRenderer = uiRenderer.GetTextRenderer();
	char            tmp[256];
	const TextStyle textStyle { whiteColor, blackColor };
	snprintf(tmp, sizeof(tmp), "%s", GetLocalizedString(GameStringId::youCompletedAllLevels));
	textRenderer.WriteAligned(*mFont, tmp, Vec2 { 0, 400 }, TextAlignment::center, TextDirection::leftToRight, textStyle, GameDrawOrder::overUI);
	snprintf(tmp, sizeof(tmp), "%s %d", GetLocalizedString(GameStringId::yourFinalScoreIs), mMatchStats.score);
	textRenderer.WriteAligned(*mFont, tmp, Vec2 { 0, 460 }, TextAlignment::center, TextDirection::leftToRight, textStyle, GameDrawOrder::overUI);
}

void GameCompleteScreen::Enter(const ScreenNavArgs& args) {
	mAccumTime = 0.f;
}

void GameCompleteScreen::Exit() {
}

void GameCompleteScreen::ParseConfig(const char* varName, const char* varValue) {
}
