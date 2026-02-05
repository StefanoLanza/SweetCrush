#include "CreditsScreen.h"
#include "Constants.h"
#include "Localization.h"
#include "ScreenIds.h"
#include "GameUI.h"

#include <engine/Engine.h>
#include <engine/Input.h>
#include <engine/TextRender.h>
#include <engine/UI.h>

using namespace Wind;

namespace {

const UITextDesc codeByText {
	.pos = { 0, 560 },
	.horizontalAlignment = UIHorizAlignment::center,
	.verticalAlignment = UIVertAlignment::top,
	.font = "smallFont",
	.stringId = GameStringId::codeBy,
};

const UITextDesc graphicsByText {
	.pos = { 0, 620 },
	.horizontalAlignment = UIHorizAlignment::center,
	.verticalAlignment = UIVertAlignment::top,
	.font = "smallFont",
	.stringId = GameStringId::graphicsBy,
};

const UITextDesc musicByText {
	.pos = { 0, 680 },
	.horizontalAlignment = UIHorizAlignment::center,
	.verticalAlignment = UIVertAlignment::top,
	.font = "smallFont",
	.stringId = GameStringId::musicBy,
};

const UITextDesc versionText {
	.pos = { 0, 840 },
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
    : mTitle { MakeTitleText(GameStringId::credits) }
    , mCodeBy { MakeScreenText(GameStringId::codeBy, 560) }
    , mGraphicsBy { MakeScreenText(GameStringId::graphicsBy, 620) }
    , mMusicBy { MakeScreenText(GameStringId::musicBy, 680) }
    , mVersion { MakeScreenText(GameStringId::version, 840) }
	, mBackButton{ MakeBackButton() }
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
	    mBackButton.IsClicked(input)) {
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
