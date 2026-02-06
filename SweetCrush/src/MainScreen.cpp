#include "MainScreen.h"
#include "AssetDefs.h"
#include "Constants.h"
#include "GameDrawOrder.h"
#include "GameRenderer.h"
#include "GameUI.h"
#include "Localization.h"
#include "ScreenIds.h"

#include <engine/BitmapRender.h>
#include <engine/Easings.h>
#include <engine/Engine.h>
#include <engine/Input.h>
#include <engine/TextRender.h>
#include <engine/UI.h>
#include <engine/UIRenderer.h>

using namespace Wind;

namespace {

constexpr UICanvasDesc canvasDesc {
	.background = "gameartguppy/background.png",
};

} // namespace

MainScreen::MainScreen(Engine& engine, const GameRenderer& gameRenderer)
    : mEngine(engine)
    , mGameRenderer(gameRenderer)
    , mTitle { MakeTitleText(GameStringId::title) }
    , mStartButton { MakeMenuButton(button0_y, GameStringId::start) }
    , mSettingsButton { MakeMenuButton(button1_y, GameStringId::settings) }
    , mCreditsButton { MakeMenuButton(button2_y, GameStringId::credits) }
#if ! defined(__ANDROID__) && ! defined(__OHOS__)
    , mQuitButton { MakeQuitButton() }
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
	mCanvas.LoadAssets(engine.GetGraphics(), engine.GetFontManager());
}

ScreenEvent MainScreen::Tick(float dt, const Wind::Input& input) {
	mTime += dt;

	if (mStartButton.IsClicked(input)) {
		return GoTo(GameScreenIds::levelStart, ScreenTransition::slideTop);
	}
	else if (mSettingsButton.IsClicked(input)) {
		return GoTo(GameScreenIds::settings, ScreenTransition::slideLeft);
	}
	else if (mCreditsButton.IsClicked(input)) {
		return GoTo(GameScreenIds::credits, ScreenTransition::slideLeft);
	}

	AnimateUI();

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

#if 0
	{
		BitmapExtParams prm;
		prm.orientation = std::sin(0.f + (float)mTime * 2.f) * 0.05f;
		prm.drawOrder = GameDrawOrder::overUI;
		prm.pivot = { 0.4f, 1.f };
		prm.blending = true;
		mEngine.GetBitmapRenderer().DrawBitmapEx(*gameTextures[petSprites[0]], { RefWindowWidth - 100.f, RefWindowHeight - 0.f }, prm);
	}
#endif
}

void MainScreen::Enter(const ScreenNavArgs& args) {
	mTime = 0.f;
	AnimateUI();
}

void MainScreen::Exit() {
}

void MainScreen::ParseConfig(const char* varName, const char* varValue) {
}

void MainScreen::AnimateUI() {
	float t = std::min(1.f, mTime * 3.f);
	auto  desc = mSettingsButton.GetDesc();
	//	mQuitButton.GetDesc Bitmap()->SetColor(Color { desc.color.r, desc.color.g, desc.color.b, 255.f * t });
	// desc.scale = LerpEase(0.85f, 1.f, t, EaseOutBounce);
	desc.pos.ax = LerpEase(-400.f, 0.f, t, EaseOutCubic);
	// mSettingsButton.SetDesc(desc);
	desc = mStartButton.GetDesc();
	desc.pos.ax = LerpEase(400.f, 0.f, t, EaseOutCubic);
	// mStartButton.SetDesc(desc);
}
