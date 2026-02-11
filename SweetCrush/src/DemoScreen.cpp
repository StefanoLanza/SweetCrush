#include "DemoScreen.h"
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

DemoScreen::DemoScreen(Engine& engine, const GameRenderer& gameRenderer)
    : mEngine(engine)
    , mGameRenderer(gameRenderer)
    , mTitle { MakeTitleText(GameStringId::title) }
    , mCanvas(canvasDesc)
    , mAccumTime(0) {
	// Setup UI
	mCanvas.Add(mTitle);
}

const char* DemoScreen::GetName() const {
	return "DemoScreen";
}

void DemoScreen::LoadAssets(Engine& engine) {
	mCanvas.LoadAssets(engine.GetGraphics(), engine.GetFontManager());
}

ScreenEvent DemoScreen::Tick(float dt, const Wind::Input& input) {
	mCanvas.HandleInput(input);
	mAccumTime += dt;
	if (mAccumTime > 8.f) {
		GoBack();
	}

	return Continue();
}

void DemoScreen::Draw(UIRenderer& uiRenderer, float dt) {
	mCanvas.Draw(RefWindowWidth, RefWindowHeight, uiRenderer, 0);
}

void DemoScreen::Enter(const ScreenNavArgs& args) {
	mAccumTime = 0.f;
}

void DemoScreen::Exit() {
}

void DemoScreen::ParseConfig(const char* varName, const char* varValue) {
}

void DemoScreen::AnimateUI() {
}
