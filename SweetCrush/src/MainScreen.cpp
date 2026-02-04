#include "MainScreen.h"
#include "AssetDefs.h"
#include "Constants.h"
#include "GameDrawOrder.h"
#include "GameRenderer.h"
#include "Localization.h"
#include "ScreenIds.h"
#include "UIDefs.h"

#include <engine/BitmapRender.h>
#include <engine/Engine.h>
#include <engine/Input.h>
#include <engine/TextRender.h>
#include <engine/UI.h>
#include <engine/UIRenderer.h>

using namespace Wind;

namespace {

#if defined(__ANDROID__) || defined(__OHOS__)
// No quit button on mobiles
const UIButtonDesc buttonDescs[] {
	{ UIAbsolutePos(0, button0_y), defaultButtonSize, UIHorizAlignment::center, UIVertAlignment::top },
	{ UIAbsolutePos(0, button1_y), defaultButtonSize, UIHorizAlignment::center, UIVertAlignment::top },
	{ UIAbsolutePos(0, button2_y), defaultButtonSize, UIHorizAlignment::center, UIVertAlignment::top },
};
#else
const UIButtonDesc buttonDescs[] {
	{ UIAbsolutePos(0, button0_y), defaultButtonSize, UIHorizAlignment::center, UIVertAlignment::top },
	{ UIAbsolutePos(0, button1_y), defaultButtonSize, UIHorizAlignment::center, UIVertAlignment::top },
	{ UIAbsolutePos(0, button2_y), defaultButtonSize, UIHorizAlignment::center, UIVertAlignment::top },
};
#endif
const UITextDesc textDescs[5] {
	{
		.pos = { 0.f, titleY },
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::top,
	    .font = "bigFont",
	    .stringId = GameStringId::title,
	    .textStyle = titleTextStyle,
	},
	{
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::center,
	    .font = "mediumFont",
	    .stringId = GameStringId::start,
	},
	{
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::center,
	    .font = "mediumFont",
	    .stringId = GameStringId::settings,
	},
	{
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::center,
	    .font = "mediumFont",
	    .stringId = GameStringId::credits,
	},
	{
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::center,
	    .font = "mediumFont",
	    .stringId = GameStringId::quit,
	},
};

constexpr UICanvasDesc canvasDesc {
	.background = "gameartguppy/background.png",
};

} // namespace

MainScreen::MainScreen(Engine& engine, const GameRenderer& gameRenderer)
    : mEngine(engine)
    , mGameRenderer(gameRenderer)
    , mTitle(textDescs[0])
    , mStartButton(buttonDescs[0], buttonBitmapDesc, textDescs[1])
    , mSettingsButton(buttonDescs[1], buttonBitmapDesc, textDescs[2])
    , mCreditsButton(buttonDescs[2], buttonBitmapDesc, textDescs[3])
#if ! defined(__ANDROID__) && ! defined(__OHOS__)
    , mQuitButton(defaultBackButtonDesc, defaultBackButtonBitmapDesc)
#endif
    , mCanvas(canvasDesc)
    , mTime(0) {
	// Setup UI
	mCanvas.AddText(mTitle);
	mCanvas.AddButton(mStartButton);
	mCanvas.AddButton(mSettingsButton);
	mCanvas.AddButton(mCreditsButton);
#if ! defined(__ANDROID__) && ! defined(__OHOS__)
	mCanvas.AddButton(mQuitButton);
#endif
}

const char* MainScreen::GetName() const {
	return "MainScreen";
}

void MainScreen::LoadAssets(Engine& engine) {
	mCanvas.LoadAssets(engine.GetGraphics(), engine.GetTextRenderer());
}

ScreenEvent MainScreen::Tick(float dt, const Wind::Input& input) {
	mTime += dt;
	if (mStartButton.IsClicked(input)) {
		return GoTo(GameScreenIds::levelStart, ScreenTransition::zoomInOut);
	}
	else if (mSettingsButton.IsClicked(input)) {
		return GoTo(GameScreenIds::settings, ScreenTransition::slideIn);
	}
	else if (mCreditsButton.IsClicked(input)) {
		return GoTo(GameScreenIds::credits, ScreenTransition::slideIn);
	}

#if defined(__ANDROID__) || defined(__OHOS__)
	if (input.GetKeyJustPressed(SDLK_AC_BACK)) {
		mEngine.Quit();
#elif defined(_WIN32) || defined(__linux__)
	if (input.GetKeyJustPressed(SDLK_ESCAPE) || mQuitButton.IsClicked(input)) {
#endif
		mEngine.Quit();
	}
	return Continue();
}

void MainScreen::Draw(UIRenderer& uiRenderer, float dt) {
	mCanvas.Draw(RefWindowWidth, RefWindowHeight, uiRenderer, 0);

	constexpr float dx = TileWidth + 2;
	float           phase = mTime * 4.f;
	float           x = (RefWindowWidth - (NumPieceTypes - 1) * dx) * 0.5f;

	for (int i = 0; i < NumPieceTypes; ++i) {
		BitmapExtParams prm;
		prm.pivot = BitmapPivot::center;
		prm.orientation = std::sin(phase * .25f + (float)i) * 0.5f;
		prm.drawOrder = GameDrawOrder::overUI;
		prm.blending = true;
		mEngine.GetBitmapRenderer().DrawBitmapEx(*gameTextures[pieceIcons[i]], { x, 380.f + std::cos(phase) * 4.f }, prm);
		x += dx;
		phase += 6.28f / static_cast<float>(NumPieceTypes);
	}

	{
		BitmapExtParams prm;
		prm.orientation = std::sin(0.f + (float)mTime * 2.f) * 0.05f;
		prm.drawOrder = GameDrawOrder::overUI;
		prm.pivot = { 0.4f, 1.f };
		prm.blending = true;
		mEngine.GetBitmapRenderer().DrawBitmapEx(*gameTextures[petSprites[0]], { RefWindowWidth - 100.f, RefWindowHeight - 0.f }, prm);
	}
}

void MainScreen::Enter(const ScreenNavArgs& args) {
}

void MainScreen::Exit() {
}

void MainScreen::ParseConfig(const char* varName, const char* varValue) {
}
