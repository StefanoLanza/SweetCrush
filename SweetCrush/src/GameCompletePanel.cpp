#include "GameCompletePanel.h"
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
	    "bigFont",
	    (StringId)GameStringId::gameComplete,
	    UIAbsolutePos(0, titleY),
	    UIAutoSize,
	    UIHorizAlignment::center,
	    UIVertAlignment::top,
	    titleTextStyle,
	},
	{
	    "mediumFont",
	    (StringId)GameStringId::continueGame,
	    UIZeroPos,
	    UIAutoSize,
	    UIHorizAlignment::center,
	    UIVertAlignment::center,
	    defaultTextStyle,
	},
};

const UIBitmapDesc panelBitmapDesc {
	"null.png", UIZeroPos, UIParentSize, UIHorizAlignment::center, UIVertAlignment::center, Color { 0, 0, 0, 200 }, UIBlending::on, 0,
};

} // namespace

GameCompletePanel::GameCompletePanel(Engine& engine, const MatchStats& matchStats)
    : mEngine(engine)
    , mMatchStats(matchStats)
    , mTitle(textDescs[0], engine)
    , mContinueButton(MakeButton(buttonDescs[0], buttonBitmapDesc, textDescs[1], engine))
    , mPanelBitmap(panelBitmapDesc, engine)
    , mPanel(UIDefaultPanelDesc) {
}

void GameCompletePanel::LoadAssets() {
	mFont = mEngine.GetTextRenderer().AddFont("smallFont");
}

void GameCompletePanel::BuildUI(UICanvas& canvas) {
	mPanel.AddText(mTitle);
	mPanel.AddButton(mContinueButton);
	// mPanel.AddBitmap(mPanelBitmap);
	canvas.GetPanel().AddPanel(mPanel);
}

GameScreenId GameCompletePanel::Tick(float /*dt*/, const Wind::Input& input) {
	if (mContinueButton.IsPressed(input)) {
		return ScreenId::mainMenu;
	}
	return ScreenId::gameComplete;
}

void GameCompletePanel::Draw(GameScreenId topScreen) const {
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
	textRenderer.WriteAligned(*mFont, tmp, Vec2 { 0, 400 }, TextAlignment::center, textStyle, (DrawOrder)GameDrawOrder::textOverUI);
	snprintf(tmp, sizeof(tmp), "%s %d", GetLocalizedString(GameStringId::yourFinalScoreIs), mMatchStats.score);
	textRenderer.WriteAligned(*mFont, tmp, Vec2 { 0, 460 }, TextAlignment::center, textStyle, (DrawOrder)GameDrawOrder::textOverUI);
}

void GameCompletePanel::Enter([[maybe_unused]] GameScreenId prevScreen) {
	mPanel.SetVisible(true);
}

void GameCompletePanel::Exit() {
	mPanel.SetVisible(false);
}
