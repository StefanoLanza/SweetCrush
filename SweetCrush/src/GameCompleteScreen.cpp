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
	    .stringId=(StringId)GameStringId::gameComplete,
	    .textStyle=titleTextStyle,
	},
	{
	    .pos = UIZeroPos,
	    .size = UIAutoSize,
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::center,
	    .font = "mediumFont",
	    .stringId=(StringId)GameStringId::continueGame,
	},
};

const UIBitmapDesc panelBitmapDesc {
	"null.png", UIZeroPos, UIParentSize, UIHorizAlignment::center, UIVertAlignment::center, Color { 0, 0, 0, 200 }, UIBlending::on, 0,
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

GameScreenId GameCompleteScreen::Tick(float /*dt*/, const Wind::Input& input) {
	if (mContinueButton.IsPressed(input)) {
		return ScreenId::mainMenu;
	}
	return ScreenId::gameComplete;
}

void GameCompleteScreen::Draw(GameScreenId topScreen) const {
	if (topScreen != ScreenId::gameComplete) {
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

void GameCompleteScreen::Enter([[maybe_unused]] GameScreenId prevScreen, const void* payload) {
	mPanel.SetVisible(true);
}

void GameCompleteScreen::Exit() {
	mPanel.SetVisible(false);
}

void GameCompleteScreen::ParseConfig(const char* varName, const char* varValue) {
}
