#include "CreditsScreen.h"
#include "Localization.h"
#include "ScreenIds.h"
#include "UIDefs.h"
#include "Constants.h"

#include <engine/Engine.h>
#include <engine/Input.h>
#include <engine/TextRender.h>
#include <engine/UI.h>

using namespace Wind;

namespace {

const UIButtonDesc backButtonDesc { UIAbsolutePos(0, 920), UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top };

const UITextDesc titleText {

	.pos = { 0, titleY, 0, 0 },
	.size = UIAutoSize,
	.horizontalAlignment = UIHorizAlignment::center,
	.verticalAlignment = UIVertAlignment::top,
	.font = "bigFont",
	.stringId = (StringId)GameStringId::credits,
	.textStyle = titleTextStyle,
};

const UITextDesc codeByText {

	.pos = { 0, 360, 0, 0 },
	.size = UIAutoSize,
	.horizontalAlignment = UIHorizAlignment::center,
	.verticalAlignment = UIVertAlignment::top,
	.font = "smallFont",
	.stringId = (StringId)GameStringId::codeBy,
};

const UITextDesc graphicsByText {
	.pos = { 0, 420, 0, 0 },
	.size = UIAutoSize,
	.horizontalAlignment = UIHorizAlignment::center,
	.verticalAlignment = UIVertAlignment::top,
	.font = "smallFont",
	.stringId = (StringId)GameStringId::graphicsBy,
};

const UITextDesc musicByText {

	.pos = { 0, 480, 0, 0 },
	.size = UIAutoSize,
	.horizontalAlignment = UIHorizAlignment::center,
	.verticalAlignment = UIVertAlignment::top,
	.font = "smallFont",
	.stringId = (StringId)GameStringId::musicBy,
};

const UITextDesc versionText {
	.pos = { 0, 640, 0, 0 },
	.size = UIAutoSize,
	.horizontalAlignment = UIHorizAlignment::center,
	.verticalAlignment = UIVertAlignment::top,
	.font = "smallFont",
	.stringId = (StringId)GameStringId::version,
};

const UITextDesc backText {
	.pos = UIZeroPos,
	.size = UIAutoSize,
	.horizontalAlignment = UIHorizAlignment::center,
	.verticalAlignment = UIVertAlignment::center,
	.font = "mediumFont",
	.stringId = (StringId)GameStringId::back,
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
    , mBackButton(MakeButton(backButtonDesc, buttonBitmapDesc, backText))
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
