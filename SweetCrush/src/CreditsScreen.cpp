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

const UIPanelDesc panelDesc {
	.pos = UIAbsolutePos(0.f, 0.f),
	.size = UIAbsoluteSize(560.f, 500.f),
	.horizontalAlignment = UIHorizAlignment::center,
	.verticalAlignment = UIVertAlignment::center,
	.background = "UI/button.png",
	.backgroundColor = panel0_color,
	._9patch = 16.f,
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
    , mPanel(panelDesc) {
	mCanvas.AddText(mTitle);
	mCanvas.AddPanel(mPanel);
	mPanel.AddText(mCodeBy);
	mPanel.AddText(mGraphicsBy);
	mPanel.AddText(mMusicBy);
	mPanel.AddText(mVersion);
	mCanvas.AddButton(mBackButton);
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
