#include "GameOverScreen.h"
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
	{
	    UIAbsolutePos(0, 800),
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
	    .stringId = (StringId)GameStringId::gameOver,
	    .textStyle = titleTextStyle,
	},
	{

	    .pos = UIZeroPos,
	    .size = UIAutoSize,
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::center,
	    .font = "mediumFont",
	    .stringId = (StringId)GameStringId::retry,
	},
	{
	    .pos = UIZeroPos,
	    .size = UIAutoSize,
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::center,
	    .font = "mediumFont",
	    .stringId = (StringId)GameStringId::toMainMenu,
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

const char* GameOverScreen::GetName() const {
	return "GameOverScreen";
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

ScreenTransition GameOverScreen::Tick(float /*dt*/, const Wind::Input& input) {
	if (mContinueButton.IsPressed(input)) {
		return { ScreenOp::replace, GameScreenIds::mainMenu };
	}
	else if (mReplayLevelButton.IsPressed(input)) {
		return { ScreenOp::replace, GameScreenIds::play };
	}
	return { ScreenOp::keep };
}

void GameOverScreen::Draw(ScreenId topScreen) const {
	if (topScreen != GameScreenIds::gameOver) {
		return;
	}
	if (! mFont) {
		return;
	}
	const auto&     textRenderer = mEngine.GetTextRenderer();
	char            tmp[256];
	const TextStyle textStyle { whiteColor, blackColor };
	snprintf(tmp, sizeof(tmp), "%s %d", GetLocalizedString(GameStringId::yourReachedLevel), mMatchStats.level + 1);
	textRenderer.WriteAligned(*mFont, tmp, Vec2 { 0, 470 }, TextAlignment::center, textStyle, (DrawOrder)GameDrawOrder::overUI);
	snprintf(tmp, sizeof(tmp), "%s %d", GetLocalizedString(GameStringId::yourScoreIs), mMatchStats.score);
	textRenderer.WriteAligned(*mFont, tmp, Vec2 { 0, 530 }, TextAlignment::center, textStyle, (DrawOrder)GameDrawOrder::overUI);
}

void GameOverScreen::Enter(ScreenId /*prevScreen*/, const void* payload) {
	mPanel.SetVisible(true);
}

void GameOverScreen::Exit() {
	mPanel.SetVisible(false);
}

void GameOverScreen::ParseConfig(const char* varName, const char* varValue) {
}
