#include "MainScreen.h"
#include "AssetDefs.h"
#include "Constants.h"
#include "GameDrawOrder.h"
#include "GameRenderer.h"
#include "Localization.h"
#include "ScreenIds.h"
#include "UIDefs.h"

#include <engine/Engine.h>
#include <engine/Input.h>
#include <engine/TextRender.h>
#include <engine/UI.h>

#include <cmath>

using namespace Wind;

namespace {

#if defined(__ANDROID__) || defined(__OHOS__)
// No quit button on mobiles
const UIButtonDesc buttonDescs[] {
	{ UIAbsolutePos(0, 680), UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top },
	{ UIAbsolutePos(0, 800), UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top },
	{ UIAbsolutePos(0, 920), UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top },
};
#else
const UIButtonDesc buttonDescs[] {
	{ UIAbsolutePos(0, 560), UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top },
	{ UIAbsolutePos(0, 680), UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top },
	{ UIAbsolutePos(0, 800), UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top },
	{ UIAbsolutePos(0, 920), UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top },
};
#endif
const UITextDesc textDescs[5] {
	{ .pos = UIAbsolutePos(0, titleY),
	  .size = UIAutoSize,
	  .horizontalAlignment = UIHorizAlignment::center,
	  .verticalAlignment = UIVertAlignment::top,
	  .font = "bigFont",
	  .stringId = (StringId)GameStringId::title,
	  .textStyle = titleTextStyle },
	{
	    .pos = UIZeroPos,
	    .size = UIAutoSize,
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::center,
	    .font = "mediumFont",
	    .stringId = (StringId)GameStringId::start,
	},
	{
	    .pos = UIZeroPos,
	    .size = UIAutoSize,
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::center,
	    .font = "mediumFont",
	    .stringId = (StringId)GameStringId::settings,
	},
	{
	    .pos = UIZeroPos,
	    .size = UIAutoSize,
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::center,
	    .font = "mediumFont",
	    .stringId = (StringId)GameStringId::credits,
	},
	{
	    .pos = UIZeroPos,
	    .size = UIAutoSize,
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::center,
	    .font = "mediumFont",
	    .stringId = (StringId)GameStringId::quit,
	},
};

} // namespace

MainScreen::MainScreen(Engine& engine, const GameRenderer& gameRenderer)
    : mEngine(engine)
    , mGameRenderer(gameRenderer)
    , mTitle(textDescs[0], engine)
    , mStartButton(MakeButton(buttonDescs[0], buttonBitmapDesc, textDescs[1], engine))
    , mSettingsButton(MakeButton(buttonDescs[1], buttonBitmapDesc, textDescs[2], engine))
    , mCreditsButton(MakeButton(buttonDescs[2], buttonBitmapDesc, textDescs[3], engine))
#if ! defined(__ANDROID__) && ! defined(__OHOS__)
    , mQuitButton(MakeButton(buttonDescs[3], buttonBitmapDesc, textDescs[4], engine))
#endif
    , mPanel(UIDefaultPanelDesc)
    , mTime(0) {
}

const char* MainScreen::GetName() const {
	return "MainScreen";
}

void MainScreen::LoadAssets() {
}

void MainScreen::BuildUI(UICanvas& canvas) {
	mPanel.AddText(mTitle);
	mPanel.AddButton(mStartButton);
	mPanel.AddButton(mSettingsButton);
	mPanel.AddButton(mCreditsButton);
#if ! defined(__ANDROID__) && ! defined(__OHOS__)
	mPanel.AddButton(mQuitButton);
#endif
	canvas.GetPanel().AddPanel(mPanel);
}

GameScreenId MainScreen::Tick(float dt, const Wind::Input& input) {
	mTime += dt;
	if (mStartButton.IsPressed(input)) {
		return ScreenId::play;
	}
	else if (mSettingsButton.IsPressed(input)) {
		return ScreenId::settings;
	}
	else if (mCreditsButton.IsPressed(input)) {
		return ScreenId::credits;
	}

#if defined(__ANDROID__) || defined(__OHOS__)
	if (input.GetKeyPressed(SDLK_AC_BACK)) {
		mEngine.Quit();
#elif defined(_WIN32) || defined(__linux__)
	if (input.GetKeyPressed(SDLK_ESCAPE) || mQuitButton.IsPressed(input)) {
#endif
		mEngine.Quit();
	}
	return ScreenId::mainMenu;
}

void MainScreen::Draw([[maybe_unused]] GameScreenId topScreen) const {
	if (! mPanel.IsVisible()) {
		return;
	}
	constexpr float dx = TileWidth + 2;
	float           phase = mTime * 4.f;
	float           x = (RefWindowWidth - (NumPieceTypes - 1) * dx) * 0.5f;

	for (int i = 0; i < NumPieceTypes; ++i) {
		float rotation = std::sin(phase * .25f + (float)i) * 0.5f;
		mGameRenderer.DrawIcon(pieceIcons[i], { x, 380.f + std::cos(phase) * 4.f }, rotation, whiteColor,
		                       static_cast<unsigned>(GameDrawOrder::overlays));
		x += dx;
		phase += 6.28f / static_cast<float>(NumPieceTypes);
	}
}

void MainScreen::Enter([[maybe_unused]] GameScreenId prevScreen, const void* payload) {
	mPanel.SetVisible(true);
}

void MainScreen::Exit() {
	mPanel.SetVisible(false);
}

void MainScreen::ParseConfig(const char* varName, const char* varValue) {
}
