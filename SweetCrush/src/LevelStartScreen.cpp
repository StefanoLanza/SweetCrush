#include "LevelStartScreen.h"
#include "Constants.h"
#include "Localization.h"
#include "MatchStats.h"
#include "ScreenIds.h"
#include "UIDefs.h"
#include "GameDrawOrder.h"

#include <engine/Engine.h>
#include <engine/TextRender.h>
#include <engine/UI.h>
#include <engine/UIRenderer.h>

using namespace Wind;

namespace {

const UIButtonDesc buttonDescs[] {
	{
	    UIAbsolutePos(0, 560),
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
	    .stringId = (StringId)GameStringId::goal,
	    .textStyle = titleTextStyle,
	},
	{
	    .pos = UIZeroPos,
	    .size = UIAutoSize,
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::center,
	    .font = "mediumFont",
	    .stringId = (StringId)GameStringId::start,
	    .textStyle = defaultTextStyle,
	},
};

constexpr UICanvasDesc canvasDesc {
	.background = "gameartguppy/background.png",
};

} // namespace

LevelStartScreen::LevelStartScreen(const MatchStats& matchStats)
    : mMatchStats(matchStats)
    , mTitle(textDescs[0])
    , mPlayButton(MakeButton(buttonDescs[0], buttonBitmapDesc, textDescs[1]))
    , mCanvas(canvasDesc) {
	// Setup UI
	mCanvas.AddText(mTitle);
	mCanvas.AddButton(mPlayButton);
}

const char* LevelStartScreen::GetName() const {
	return "LevelStartScreen";
}

void LevelStartScreen::LoadAssets(Engine& engine) {
	mCanvas.LoadAssets(engine.GetGraphics(), engine.GetTextRenderer());
	mFont = engine.GetTextRenderer().AddFont("smallFont");
}

ScreenEvent LevelStartScreen::Tick(float dt, const Input& input) {
	mAccumTime += dt;
	if (mAccumTime > 4.f || mPlayButton.IsPressed(input)) {
		return GoTo(GameScreenIds::play);
	}
	return Continue();
}

void LevelStartScreen::Draw(UIRenderer& uiRenderer, float dt) {
	mCanvas.Draw(RefWindowWidth, RefWindowHeight, uiRenderer, 0);

	if (! mFont) {
		return;
	}
	const auto&     textRenderer = uiRenderer.GetTextRenderer();
	char            tmp[256];
	const TextStyle textStyle {
		.color = whiteColor,
		.outlineColor = blackColor,
	};
	snprintf(tmp, sizeof(tmp), "%s", GetLocalizedString(GameStringId::goal));
	textRenderer.WriteAligned(*mFont, tmp, Vec2 { 0, 400 }, TextAlignment::center, textStyle, GameDrawOrder::overUI);
	//snprintf(tmp, sizeof(tmp), "%s %d", GetLocalizedString(GameStringId::yourFinalScoreIs), mMatchStats.score);
	//textRenderer.WriteAligned(*mFont, tmp, Vec2 { 0, 460 }, TextAlignment::center, textStyle, GameDrawOrder::overUI);
}

void LevelStartScreen::Enter(const ScreenNavArgs& args) {
	mAccumTime = 0.f;
}

void LevelStartScreen::Exit() {
}

void LevelStartScreen::ParseConfig(const char* varName, const char* varValue) {
}
