#include "CreditsScreen.h"
#include "Constants.h"
#include "GameUI.h"
#include "Localization.h"
#include "ScreenIds.h"

#include <engine/Engine.h>
#include <engine/Input.h>

using namespace Wind;

namespace {

constexpr UICanvasDesc canvasDesc {
	.background = "gameartguppy/background.png",
};


} // namespace

CreditsScreen::CreditsScreen()
    : mTitle { MakeTitleText(GameStringId::credits) }
    , mCodeBy { MakeScreenText(GameStringId::codeBy, 80) }
    , mGraphicsBy { MakeScreenText(GameStringId::graphicsBy, 160) }
    , mMusicBy { MakeScreenText(GameStringId::musicBy, 240) }
    , mVersion { MakeScreenText(GameStringId::version, 380) }
    , mBackButton { MakeBackButton() }
    , mCanvas(canvasDesc)
    , mPanel(MakeInfoPanel()) {
	mCanvas.Add(mTitle);
	mCanvas.Add(mPanel);
	mPanel.Add(mCodeBy);
	mPanel.Add(mGraphicsBy);
	mPanel.Add(mMusicBy);
	mPanel.Add(mVersion);
	mCanvas.Add(mBackButton);
}

const char* CreditsScreen::GetName() const {
	return "CreditsScreen";
}

ScreenEvent CreditsScreen::Tick(float /*dt*/, const Wind::Input& input) {
	mCanvas.HandleInput(input);
#if defined(__ANDROID__) || defined(__OHOS__)
	if (input.GetKeyJustPressed(SDLK_AC_BACK) ||
#elif defined(_WIN32) || defined(__linux__)
	if (input.GetKeyJustPressed(SDLK_ESCAPE) ||
#endif
	    mBackButton.IsClicked()) {
		return GoBack(ScreenTransition::slideRight);
	}
	return Continue();
}

void CreditsScreen::Draw(Wind::UIRenderer& uiRenderer, float dt) {
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
