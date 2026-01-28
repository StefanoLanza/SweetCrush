#include "CreditsScreen.h"
#include "Constants.h"
#include "Localization.h"
#include "ScreenIds.h"
#include "UIDefs.h"

#include <engine/Engine.h>
#include <engine/Input.h>
#include <engine/TextRender.h>
#include <engine/UI.h>

using namespace Wind;

namespace {

const UITextDesc titleText {

	.pos = { 0, titleY, 0, 0 },
	.size = UIAutoSize,
	.horizontalAlignment = UIHorizAlignment::center,
	.verticalAlignment = UIVertAlignment::top,
	.font = "screenTitle",
	.stringId = GameStringId::credits,
	.textStyle = titleTextStyle,
};

const UITextDesc codeByText {

	.pos = { 0, 560, 0, 0 },
	.size = UIAutoSize,
	.horizontalAlignment = UIHorizAlignment::center,
	.verticalAlignment = UIVertAlignment::top,
	.font = "smallFont",
	.stringId = GameStringId::codeBy,
};

const UITextDesc graphicsByText {
	.pos = { 0, 620, 0, 0 },
	.size = UIAutoSize,
	.horizontalAlignment = UIHorizAlignment::center,
	.verticalAlignment = UIVertAlignment::top,
	.font = "smallFont",
	.stringId = GameStringId::graphicsBy,
};

const UITextDesc musicByText {

	.pos = { 0, 680, 0, 0 },
	.size = UIAutoSize,
	.horizontalAlignment = UIHorizAlignment::center,
	.verticalAlignment = UIVertAlignment::top,
	.font = "smallFont",
	.stringId = GameStringId::musicBy,
};

const UITextDesc versionText {
	.pos = { 0, 840, 0, 0 },
	.size = UIAutoSize,
	.horizontalAlignment = UIHorizAlignment::center,
	.verticalAlignment = UIVertAlignment::top,
	.font = "smallFont",
	.stringId = GameStringId::version,
};

constexpr UICanvasDesc canvasDesc {
	.background = "gameartguppy/background.png",
};

} // namespace

CreditsScreen::CreditsScreen()
    : mTitle(titleText)
    , mCodeBy(codeByText)
    , mGraphicsBy(graphicsByText)
    , mMusicBy(musicByText)
    , mVersion(versionText)
    , mBackButton(defaultBackButtonDesc, defaultBackButtonBitmapDesc)
    , mCanvas(canvasDesc) {
	mCanvas.AddText(mTitle);
	mCanvas.AddText(mCodeBy);
	mCanvas.AddText(mGraphicsBy);
	mCanvas.AddText(mMusicBy);
	mCanvas.AddText(mVersion);
	mCanvas.AddButton(mBackButton);
}

const char* CreditsScreen::GetName() const {
	return "CreditsScreen";
}

ScreenEvent CreditsScreen::Tick(float /*dt*/, const Wind::Input& input) {
#if defined(__ANDROID__) || defined(__OHOS__)
	if (input.GetKeyJustPressed(SDLK_AC_BACK) ||
#elif defined(_WIN32) || defined(__linux__)
	if (input.GetKeyJustPressed(SDLK_ESCAPE) ||
#endif
	    mBackButton.IsPressed(input)) {
		return { ScreenOp::goTo, GameScreenIds::mainMenu };
	}
	return { ScreenOp::keep };
}

void CreditsScreen::Draw(Wind::UIRenderer& uiRenderer, float dt) {
	mCanvas.Draw(RefWindowWidth, RefWindowHeight, uiRenderer, 0);
}

void CreditsScreen::Enter(const ScreenNavArgs& args) {
}

void CreditsScreen::Exit() {
}

void CreditsScreen::LoadAssets(Engine& engine) {
	mCanvas.LoadAssets(engine.GetGraphics(), engine.GetTextRenderer());
}

void CreditsScreen::ParseConfig(const char* varName, const char* varValue) {
}
