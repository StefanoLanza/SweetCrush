#include "GameCompleteScreen.h"
#include "Constants.h"
#include "GameDrawOrder.h"
#include "Localization.h"
#include "MatchStats.h"
#include "ScreenIds.h"
#include "GameUI.h"

#include <engine/Engine.h>
#include <engine/FontManager.h>
#include <engine/TextRender.h>
#include <engine/UI.h>
#include <engine/UIRenderer.h>

#include <cstdio>

using namespace Wind;

namespace {

const UIButtonDesc buttonDescs[] {
	{
	    UIAbsolutePos(0, 680),
	    UIAutoSize,
	    UIHorizAlignment::center,
	    UIVertAlignment::top,
	},
};

const UITextDesc textDescs[] {
	{
	    .pos = { 0.f, titleY },
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::top,
	    .font = "screenTitle",
	    .stringId = GameStringId::gameComplete,
	},
	{
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::center,
	    .font = "mediumFont",
	    .stringId = GameStringId::continueGame,
	},
};

const UIBitmapDesc panelBitmapDesc {
	.fileName = "null.png",
	.pos = UIZeroPos,
	.size = UIParentSize,
	.horizontalAlignment = UIHorizAlignment::center,
	.verticalAlignment = UIVertAlignment::center,
	//	.color = Color { 0, 0, 0, 200 },
};

constexpr UICanvasDesc canvasDesc {
	.background = "gameartguppy/background.png",
};

} // namespace

GameCompleteScreen::GameCompleteScreen(const MatchStats& matchStats)
    : mMatchStats(matchStats)
    , mTitle(textDescs[0], titleTextStyle)
    , mContinueButton(buttonDescs[0], buttonBitmapDesc, textDescs[1])
    , mPanelBitmap(panelBitmapDesc)
    , mCanvas(canvasDesc) {
	mCanvas.AddText(mTitle);
	mCanvas.AddButton(mContinueButton);
	// mmCanvas.AddBitmap(mPanelBitmap);
}

const char* GameCompleteScreen::GetName() const {
	return "GameCompleteScreen";
}

void GameCompleteScreen::LoadAssets(Engine& engine) {
	mCanvas.LoadAssets(engine.GetGraphics(), engine.GetFontManager());
	mFont = engine.GetFontManager().AddFont("smallFont");
}

ScreenEvent GameCompleteScreen::Tick(float dt, const Wind::Input& input) {
	mAccumTime += dt;
	if (mAccumTime > 4.f || mContinueButton.IsClicked(input)) {
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
