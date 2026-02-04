#include "SettingsScreen.h"
#include "AppConfig.h"
#include "Localization.h"
#include "ScreenIds.h"
#include "UIDefs.h"

#include <engine/Engine.h>
#include <engine/Input.h>
#include <engine/UI.h>

using namespace Wind;

namespace {

const UIButtonDesc graphicsButtonDesc { { 0, button0_y, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top };
const UIButtonDesc audioButtonDesc { { 0, button1_y, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top };
const UIButtonDesc languageButtonDesc { { 0, button2_y, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top };

const UITextDesc textDescs[] {
	{
	    .pos = { 0, titleY },
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::top,
	    .font = "screenTitle",
	    .stringId = GameStringId::settings,
	    .textStyle = titleTextStyle,
	},
	{
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::center,
	    .font = "mediumFont",
	    .stringId = GameStringId::graphicsSettings,
	},
	{
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::center,
	    .font = "mediumFont",
	    .stringId = GameStringId::audioSettings,
	},
	{
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::center,
	    .font = "mediumFont",
	    .stringId = GameStringId::languageScreen,
	},
	{
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::center,
	    .font = "mediumFont",
	    .stringId = GameStringId::back,
	},
};

constexpr UICanvasDesc canvasDesc {
	.background = "gameartguppy/background.png",
};

} // namespace

SettingsScreen::SettingsScreen(GameSettings& gameSettings)
    : mGameConfig(gameSettings)
    , mTitle(textDescs[0])
    , mGraphicsButton(graphicsButtonDesc, buttonBitmapDesc, textDescs[1])
    , mAudioButton(audioButtonDesc, buttonBitmapDesc, textDescs[2])
    , mLanguageButton(languageButtonDesc, buttonBitmapDesc, textDescs[3])
    , mBackButton(defaultBackButtonDesc, defaultBackButtonBitmapDesc)
    , mCanvas(canvasDesc) {
	// Build UI
	mCanvas.AddText(mTitle);
	mCanvas.AddButton(mGraphicsButton);
	mCanvas.AddButton(mLanguageButton);
	mCanvas.AddButton(mAudioButton);
	mCanvas.AddButton(mBackButton);
}

const char* SettingsScreen::GetName() const {
	return "SettingsScreen";
}

void SettingsScreen::LoadAssets(Engine& engine) {
	mCanvas.LoadAssets(engine.GetGraphics(), engine.GetTextRenderer());
}

ScreenEvent SettingsScreen::Tick([[maybe_unused]] float dt, const Wind::Input& input) {
	if (mGraphicsButton.IsClicked(input)) {
		return GoTo(GameScreenIds::graphicsSettings, ScreenTransition::slideIn);
	}
	else if (mLanguageButton.IsClicked(input)) {
		return GoTo(GameScreenIds::language, ScreenTransition::slideIn);
	}
	else if (mAudioButton.IsClicked(input)) {
		return GoTo(GameScreenIds::audioSettings, ScreenTransition::slideIn);
	}

#if defined(__ANDROID__) || defined(__OHOS__)
	if (input.GetKeyJustPressed(SDLK_AC_BACK) ||
#elif defined(_WIN32) || defined(__linux__)
	if (input.GetKeyJustPressed(SDLK_ESCAPE) ||
#endif
	    mBackButton.IsClicked(input)) {
		return GoBack(ScreenTransition::slideOut);
	}
	return Continue();
}

void SettingsScreen::Draw(Wind::UIRenderer& uiRenderer, float dt) {
	mCanvas.Draw(RefWindowWidth, RefWindowHeight, uiRenderer, 0);
}

void SettingsScreen::Enter(const ScreenNavArgs& args) {
}

void SettingsScreen::Exit() {
}

void SettingsScreen::ParseConfig(const char* varName, const char* varValue) {
}
