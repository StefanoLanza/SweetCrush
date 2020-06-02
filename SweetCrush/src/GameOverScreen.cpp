#include "GameOverScreen.h"
#include "GameDrawOrder.h"
#include "Localization.h"
#include "MatchStats.h"
#include "ScreenIds.h"
#include "UIDefs.h"
#include <engine/Engine.h>
#include <engine/TextRender.h>
#include <engine/UI.h>

using namespace Wind;

namespace {

const UIButtonDesc buttonDescs[] {
	{
	    UIAbsolutePos(0, 560),
	    UIAutoSize,
	    UIHorizAlignment::center,
	    UIVertAlignment::top,
	},
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
	    (StringId)GameStringId::gameOver,
	    UIAbsolutePos(0, titleY),
	    UIAutoSize,
	    UIHorizAlignment::center,
	    UIVertAlignment::top,
	    titleTextStyle,
	},
	{
	    "mediumFont",
	    (StringId)GameStringId::retry,
	    UIZeroPos,
	    UIAutoSize,
	    UIHorizAlignment::center,
	    UIVertAlignment::center,
	},
	{
	    "mediumFont",
	    (StringId)GameStringId::toMainMenu,
	    UIZeroPos,
	    UIAutoSize,
	    UIHorizAlignment::center,
	    UIVertAlignment::center,
	},
};

} // namespace

GameOverScreen::GameOverScreen(Engine& engine, const MatchStats& matchStats)
    : mEngine(engine)
    , mMatchStats(matchStats)
    , mTitle(textDescs[0], engine)
    , mReplayLevelButton(MakeButton(buttonDescs[0], buttonBitmapDesc, textDescs[1], engine))
    , mContinueButton(MakeButton(buttonDescs[1], buttonBitmapDesc, textDescs[2], engine))
    , mPanel(UIDefaultPanelDesc) {
}

void GameOverScreen::LoadAssets() {
	mFont = mEngine.GetTextRenderer().AddFont("smallFont");
}

void GameOverScreen::BuildUI(UICanvas& canvas) {
	mPanel.AddText(mTitle);
	mPanel.AddButton(mReplayLevelButton);
	mPanel.AddButton(mContinueButton);
	canvas.GetPanel().AddPanel(mPanel);
}

GameScreenId GameOverScreen::Tick(float /*dt*/) {
	const Input& input = mEngine.GetInput();
	if (mContinueButton.IsPressed(input)) {
		return ScreenId::mainMenu;
	}
	else if (mReplayLevelButton.IsPressed(input)) {
		return ScreenId::play;
	}
	return ScreenId::gameOver;
}

void GameOverScreen::Draw(GameScreenId topScreen) const {
	if (topScreen != ScreenId::gameOver) {
		return;
	}
	if (! mFont) {
		return;
	}
	const auto&     textRenderer = mEngine.GetTextRenderer();
	char            tmp[256];
	const TextStyle textStyle { whiteColor, blackColor };
	snprintf(tmp, sizeof(tmp), "%s %d", GetLocalizedString(GameStringId::yourReachedLevel), mMatchStats.level + 1);
	textRenderer.WriteAligned(*mFont, tmp, Vec2 { 0, 350 }, TextAlignment::center, textStyle, (DrawOrder)GameDrawOrder::textOverUI);
	snprintf(tmp, sizeof(tmp), "%s %d", GetLocalizedString(GameStringId::yourScoreIs), mMatchStats.score);
	textRenderer.WriteAligned(*mFont, tmp, Vec2 { 0, 410 }, TextAlignment::center, textStyle, (DrawOrder)GameDrawOrder::textOverUI);
}

void GameOverScreen::Enter(GameScreenId /*prevScreen*/) {
	mPanel.SetVisible(true);
}

void GameOverScreen::Exit() {
	mPanel.SetVisible(false);
}
