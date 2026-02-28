#include "CreditsScreen.h"
#include "Constants.h"
#include "GameSettings.h"
#include "GameUI.h"
#include "Localization.h"
#include "ScreenIds.h"

#include <engine/Engine.h>
#include <engine/Input.h>

using namespace Wind;

CreditsScreen::CreditsScreen(const GameSettings& gameSettings)
    : mGameSettings(gameSettings)
    , mCanvas(MakeCanvas()) {
	// Build UI
	mCanvas.Add(MakeTitle(GameStringId::credits));
	auto panel = mCanvas.Add(MakeInfoPanel());
	panel->Add(MakeScreenText(GameStringId::codeBy, 60));
	panel->Add(MakeScreenText(GameStringId::graphicsBy, 140));
	panel->Add(MakeScreenText(GameStringId::musicBy, 220));
	panel->Add(MakeScreenText(GameStringId::fontBy, 300));
	mBackButton = mCanvas.Add(MakeBackButton());
}

const char* CreditsScreen::GetName() const {
	return "CreditsScreen";
}

ScreenEvent CreditsScreen::Tick(float /*dt*/, const Input& input) {
	mCanvas.HandleInput(input);
#if defined(__ANDROID__) || defined(__OHOS__)
	if (input.GetKeyJustPressed(SDLK_AC_BACK) ||
#elif defined(_WIN32) || defined(__linux__)
	if (input.GetKeyJustPressed(SDLK_ESCAPE) ||
#endif
	    mBackButton->IsClicked()) {
		return GoBack(ScreenTransition::slideRight);
	}
	return Continue();
}

void CreditsScreen::Draw(UIRenderer& uiRenderer, float dt) {
	mCanvas.Tick(dt);
	mCanvas.Draw(RefWindowWidth, RefWindowHeight, uiRenderer, 0);
}

void CreditsScreen::Enter(const ScreenNavArgs& args) {
}

void CreditsScreen::Exit() {
}

void CreditsScreen::LoadAssets(Engine& engine) {
	mCanvas.LoadAssets(engine.GetGraphics(), engine.GetFontManager());
}

void CreditsScreen::ParseConfig(const char* varName, const char* varValue) {
}
