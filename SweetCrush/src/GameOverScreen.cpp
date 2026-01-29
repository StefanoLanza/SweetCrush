#include "GameOverScreen.h"
#include "Constants.h"
#include "GameDrawOrder.h"
#include "Localization.h"
#include "MatchStats.h"
#include "ScreenIds.h"
#include "UIDefs.h"

#include <engine/Engine.h>
#include <engine/TextRender.h>
#include <engine/UI.h>
#include <engine/UIRenderer.h>

#include <cstdio>

using namespace Wind;

namespace {

const UIButtonDesc buttonDescs[] {
	{
	    .pos = UIAbsolutePos(0, button1_y),
	    .size = UIAutoSize,
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::top,
	},
	{
	    .pos = UIAbsolutePos(0, button2_y),
	    .size = UIAutoSize,
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::top,
	},
};

const UITextDesc textDescs[] {
	{
	    .pos = UIAbsolutePos(0, titleY),
	    .size = UIAutoSize,
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::top,
	    .font = "screenTitle",
	    .stringId = GameStringId::gameOver,
	    .textStyle = titleTextStyle,
	},
	{

	    .pos = UIZeroPos,
	    .size = UIAutoSize,
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::center,
	    .font = "mediumFont",
	    .stringId = GameStringId::retry,
	},
	{
	    .pos = UIZeroPos,
	    .size = UIAutoSize,
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::center,
	    .font = "mediumFont",
	    .stringId = GameStringId::toMainMenu,
	},
};

constexpr UICanvasDesc canvasDesc {
	.background = "gameartguppy/background.png",
};

} // namespace

GameOverScreen::GameOverScreen(const MatchStats& matchStats)
    : mMatchStats(matchStats)
    , mTitle(textDescs[0])
    , mReplayLevelButton(buttonDescs[0], buttonBitmapDesc, textDescs[1])
    , mContinueButton(buttonDescs[1], buttonBitmapDesc, textDescs[2])
    , mCanvas(canvasDesc) {
	mCanvas.AddText(mTitle);
	mCanvas.AddButton(mReplayLevelButton);
	mCanvas.AddButton(mContinueButton);
}

const char* GameOverScreen::GetName() const {
	return "GameOverScreen";
}

void GameOverScreen::LoadAssets(Engine& engine) {
	mFont = engine.GetTextRenderer().AddFont("smallFont");
	mCanvas.LoadAssets(engine.GetGraphics(), engine.GetTextRenderer());
}

ScreenEvent GameOverScreen::Tick(float /*dt*/, const Wind::Input& input) {
	if (mContinueButton.IsPressed(input)) {
		return { ScreenOp::goTo, GameScreenIds::mainMenu };
	}
	else if (mReplayLevelButton.IsPressed(input)) {
		return { ScreenOp::goTo, GameScreenIds::play };
	}
	return { ScreenOp::keep };
}

void GameOverScreen::Draw(Wind::UIRenderer& uiRenderer, float dt) {
	mCanvas.Draw(RefWindowWidth, RefWindowHeight, uiRenderer, 0);
	if (! mFont) {
		return;
	}
	const auto&     textRenderer = uiRenderer.GetTextRenderer();
	char            tmp[256];
	const TextStyle textStyle { whiteColor, blackColor };
	snprintf(tmp, sizeof(tmp), "%s %d", GetLocalizedString(GameStringId::yourReachedLevel), mMatchStats.levelIndex + 1);
	textRenderer.WriteAligned(*mFont, tmp, Vec2 { 0, text0_y }, TextAlignment::center, textStyle, GameDrawOrder::overUI);
	snprintf(tmp, sizeof(tmp), "%s %d", GetLocalizedString(GameStringId::yourScoreIs), mMatchStats.score);
	textRenderer.WriteAligned(*mFont, tmp, Vec2 { 0, text1_y }, TextAlignment::center, textStyle, GameDrawOrder::overUI);
}

void GameOverScreen::Enter(const ScreenNavArgs& args) {
}

void GameOverScreen::Exit() {
}

void GameOverScreen::ParseConfig(const char* varName, const char* varValue) {
}
