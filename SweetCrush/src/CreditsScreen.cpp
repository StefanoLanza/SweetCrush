#include "CreditsScreen.h"
#include "Localization.h"
#include "ScreenIds.h"
#include "UIDefs.h"

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
	.size=UIAutoSize,
	.horizontalAlignment=UIHorizAlignment::center,
	.verticalAlignment = UIVertAlignment::top,
	.font = "smallFont",
	.stringId = (StringId)GameStringId::musicBy,
};

const UITextDesc versionText {
	.pos = { 0, 640, 0, 0 },
	.size=UIAutoSize,
	.horizontalAlignment=UIHorizAlignment::center,
	.verticalAlignment = UIVertAlignment::top,
	.font = "smallFont",
	.stringId = (StringId)GameStringId::version,
};

const UITextDesc backText {
	.pos = UIZeroPos,
	.size=UIAutoSize,
	.horizontalAlignment=UIHorizAlignment::center,
	.verticalAlignment = UIVertAlignment::center,
	.font = "mediumFont",
	.stringId = (StringId)GameStringId::back,
};

} // namespace

CreditsScreen::CreditsScreen(Engine& engine)
    : mTitle(titleText, engine)
    , mCodeBy(codeByText, engine)
    , mGraphicsBy(graphicsByText, engine)
    , mMusicBy(musicByText, engine)
    , mVersion(versionText, engine)
    , mBackButton(MakeButton(backButtonDesc, buttonBitmapDesc, backText, engine))
    , mPanel(UIDefaultPanelDesc) {
}

const char* CreditsScreen::GetName() const {
	return "CreditsScreen";
}

GameScreenId CreditsScreen::Tick(float /*dt*/, const Wind::Input& input) {
#if defined(__ANDROID__) || defined(__OHOS__)
	if (input.GetKeyPressed(SDLK_AC_BACK) ||
#elif defined(_WIN32) || defined(__linux__)
	if (input.GetKeyJustPressed(SDLK_ESCAPE) ||
#endif
	    mBackButton.IsPressed(input)) {
		return ScreenId::mainMenu;
	}
	return ScreenId::credits;
}

void CreditsScreen::Draw([[maybe_unused]] GameScreenId topScreen) const {
}

void CreditsScreen::Enter([[maybe_unused]] GameScreenId prevScreen, const void* payload) {
	mPanel.SetVisible(true);
}

void CreditsScreen::Exit(GameScreenId newScreen) {
	mPanel.SetVisible(false);
}

void CreditsScreen::LoadAssets() {
}

void CreditsScreen::BuildUI(UICanvas& canvas) {
	mPanel.AddText(mTitle);
	mPanel.AddText(mCodeBy);
	mPanel.AddText(mGraphicsBy);
	mPanel.AddText(mMusicBy);
	mPanel.AddText(mVersion);
	mPanel.AddButton(mBackButton);
	canvas.GetPanel().AddPanel(mPanel);
}

void CreditsScreen::ParseConfig(const char* varName, const char* varValue) {
}
