#include "LanguageScreen.h"
#include "AppConfig.h"
#include "GameSettings.h"
#include "Localization.h"
#include "ScreenIds.h"
#include "UIDefs.h"

#include <engine/Engine.h>
#include <engine/Input.h>
#include <engine/TextRender.h>
#include <engine/UI.h>

using namespace Wind;

namespace {

const UIButtonDesc languageButtonDesc { { 0, 560, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top };

const UITextDesc textDescs[] {
	{
	    .pos = { 0, titleY, 0, 0 },
	    .size = UIAutoSize,
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::top,
	    .font = "screenTitle",
	    .stringId = GameStringId::languageScreen,
	    .textStyle = titleTextStyle,
	},
	{

	    .pos = { 0, 0, 0, 0 },
	    .size = UIAutoSize,
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::center,
	    .font = "mediumFont",
	    .stringId = GameStringId::nextLanguage,
	},
};

constexpr UICanvasDesc canvasDesc {
	.background = "gameartguppy/background.png",
};

} // namespace

LanguageScreen::LanguageScreen()
    : mTitle(textDescs[0])
    , mLanguageButton(languageButtonDesc, buttonBitmapDesc, textDescs[1])
    , mBackButton(defaultBackButtonDesc, defaultBackButtonBitmapDesc)
    , mCanvas(canvasDesc) {
	// Setup UI
	mCanvas.AddText(mTitle);
	mCanvas.AddButton(mLanguageButton);
	mCanvas.AddButton(mBackButton);
	RefreshLanguageButton();
}

const char* LanguageScreen::GetName() const {
	return "LanguageScreen";
}

void LanguageScreen::LoadAssets(Engine& engine) {
	mCanvas.LoadAssets(engine.GetGraphics(), engine.GetTextRenderer());
}

ScreenEvent LanguageScreen::Tick([[maybe_unused]] float dt, const Wind::Input& input) {
#if defined(__ANDROID__) || defined(__OHOS__)
	if (input.GetKeyJustPressed(SDLK_AC_BACK) ||
#elif defined(_WIN32) || defined(__linux__)
	if (input.GetKeyJustPressed(SDLK_ESCAPE) ||
#endif
	    mBackButton.IsPressed(input)) {
		return GoBack(ScreenTransition::slideOut);
	}
	else if (mLanguageButton.IsPressed(input)) {
		SetNextLanguage();
	}
	return Continue();
}

void LanguageScreen::Draw(Wind::UIRenderer& uiRenderer, float dt) {
	mCanvas.Draw(RefWindowWidth, RefWindowHeight, uiRenderer, 0);
}

void LanguageScreen::Enter(const ScreenNavArgs& args) {
}

void LanguageScreen::Exit() {
}

void LanguageScreen::ParseConfig(const char* varName, const char* varValue) {
}

void LanguageScreen::RefreshLanguageButton() {
	GameStringId stringId = GameStringId::nextLanguage;
	mLanguageButton.GetText()->SetText(static_cast<StringId>(stringId));
}
