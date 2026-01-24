#include "GameCompleteScreen.h"
#include "GameDrawOrder.h"
#include "Localization.h"
#include "MatchStats.h"
#include "ScreenIds.h"
#include "UIDefs.h"
#include <engine/Engine.h>
#include <engine/TextRender.h>
#include <engine/UI.h>

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
	    .pos = UIAbsolutePos(0, titleY),
	    .size = UIAutoSize,
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::top,
	    .font = "bigFont",
	    .stringId = (StringId)GameStringId::gameComplete,
	    .textStyle = titleTextStyle,
	},
	{
	    .pos = UIZeroPos,
	    .size = UIAutoSize,
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::center,
	    .font = "mediumFont",
	    .stringId = (StringId)GameStringId::continueGame,
	},
};

const UIBitmapDesc panelBitmapDesc {
	.fileName = "null.png",
	.pos = UIZeroPos,
	.size = UIParentSize,
	.horizontalAlignment = UIHorizAlignment::center,
	.verticalAlignment = UIVertAlignment::center,
	.color = Color { 0, 0, 0, 200 },
	.blending = UIBlending::on,
};

} // namespace

GameCompleteScreen::GameCompleteScreen(Engine& engine, const MatchStats& matchStats)
    : mEngine(engine)
    , mMatchStats(matchStats)
    , mTitle(textDescs[0], engine)
    , mContinueButton(MakeButton(buttonDescs[0], buttonBitmapDesc, textDescs[1], engine))
    , mPanelBitmap(panelBitmapDesc, engine.GetGraphics())
    , mPanel(UIDefaultPanelDesc) {
}

const char* GameCompleteScreen::GetName() const {
	return "GameCompleteScreen";
}

void GameCompleteScreen::LoadAssets() {
	mFont = mEngine.GetTextRenderer().AddFont("smallFont");
}

void GameCompleteScreen::BuildUI(UICanvas& canvas) {
	mPanel.AddText(mTitle);
	mPanel.AddButton(mContinueButton);
	// mPanel.AddBitmap(mPanelBitmap);
	canvas.GetPanel().AddPanel(mPanel);
}

ScreenTransition GameCompleteScreen::Tick(float /*dt*/, const Wind::Input& input) {
	if (mContinueButton.IsPressed(input)) {
		return { ScreenOp::replace, GameScreenIds::mainMenu };
	}
	return { ScreenOp::keep };
}

void GameCompleteScreen::Draw(ScreenId topScreen) const {
	if (topScreen != GameScreenIds::gameComplete) {
		return;
	}
	if (! mFont) {
		return;
	}
	const auto&     textRenderer = mEngine.GetTextRenderer();
	char            tmp[256];
	const TextStyle textStyle { whiteColor, blackColor };
	snprintf(tmp, sizeof(tmp), "%s", GetLocalizedString(GameStringId::youCompletedAllLevels));
	textRenderer.WriteAligned(*mFont, tmp, Vec2 { 0, 400 }, TextAlignment::center, textStyle, (DrawOrder)GameDrawOrder::overUI);
	snprintf(tmp, sizeof(tmp), "%s %d", GetLocalizedString(GameStringId::yourFinalScoreIs), mMatchStats.score);
	textRenderer.WriteAligned(*mFont, tmp, Vec2 { 0, 460 }, TextAlignment::center, textStyle, (DrawOrder)GameDrawOrder::overUI);
}

void GameCompleteScreen::Enter([[maybe_unused]] ScreenId prevScreen, const void* payload) {
	mPanel.SetVisible(true);
}

void GameCompleteScreen::Exit() {
	mPanel.SetVisible(false);
}

void GameCompleteScreen::ParseConfig(const char* varName, const char* varValue) {
}
