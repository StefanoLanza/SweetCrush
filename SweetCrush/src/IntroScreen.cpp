#include "IntroScreen.h"
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
#include <engine/UIRenderer.h>

using namespace Wind;

IntroScreen::IntroScreen(Engine& engine, const GameRenderer& gameRenderer)
    : mEngine(engine)
    , mGameRenderer(gameRenderer)
    , mCanvas { MakeCanvas() }
    , mAccumTime(0) {
	// Setup UI
	mCanvas.Add( MakeTitle(GameStringId::title) );
	mCanvas.Add(MakeScreenText(GameStringId::tap, RefWindowHeight - 80.f));
}

const char* IntroScreen::GetName() const {
	return "IntroScreen";
}

void IntroScreen::LoadAssets(Engine& engine) {
	mCanvas.LoadAssets(engine.GetGraphics(), engine.GetFontManager());
}

ScreenEvent IntroScreen::Tick(float dt, const Wind::Input& input) {
	mCanvas.HandleInput(input);
	mAccumTime += dt;

#if defined(__ANDROID__) || defined(__OHOS__)
	if (input.GetFingerPressed()) {
#else
	if (input.GetMouseButtonPressed(MouseButton::left) || input.GetMouseButtonPressed(MouseButton::right)) {
#endif
		return GoTo(GameScreenIds::mainMenu); // TODO fade, ScreenTransition::slideTop);
	}

	AnimateUI();
	return Continue();
}

void IntroScreen::Draw(UIRenderer& uiRenderer, float dt) {
	mCanvas.Draw(RefWindowWidth, RefWindowHeight, uiRenderer, 0);

	constexpr float dx = TileWidth + 2;
	float           phase = mAccumTime * 4.f;
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

void IntroScreen::Enter(const ScreenNavArgs& args) {
	mAccumTime = 0.f;
	AnimateUI();
}

void IntroScreen::Exit() {
}

void IntroScreen::ParseConfig(const char* varName, const char* varValue) {
}

void IntroScreen::AnimateUI() {
}
