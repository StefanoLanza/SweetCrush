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

const UIButtonDesc graphicsButtonDesc { { 0, 560, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top };
const UIButtonDesc audioButtonDesc { { 0, 680, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top };
const UIButtonDesc languageButtonDesc { { 0, 800, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top };
const UIButtonDesc backButtonDesc { { 0, 920, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top };

const UITextDesc textDescs[] {
	{
	    .pos = { 0, titleY, 0, 0 },
	    .size = UIAutoSize,
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::top,
	    .font = "bigFont",
	    .stringId = (StringId)GameStringId::settings,
	    .textStyle = titleTextStyle,
	},
	{
	    .pos = { 0, 0, 0, 0 },
	    .size = UIAutoSize,
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::center,
	    .font = "mediumFont",
	    .stringId = (StringId)GameStringId::graphicsSettings,
	},
	{
	    .pos = { 0, 0, 0, 0 },
	    .size = UIAutoSize,
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::center,
	    .font = "mediumFont",
	    .stringId = (StringId)GameStringId::audioSettings,
	},
	{
	    .pos = { 0, 0, 0, 0 },
	    .size = UIAutoSize,
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::center,
	    .font = "mediumFont",
	    .stringId = (StringId)GameStringId::nextLanguage,
	},
	{
	    .pos = { 0, 0, 0, 0 },
	    .size = UIAutoSize,
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::center,
	    .font = "mediumFont",
	    .stringId = (StringId)GameStringId::back,
	},
};

constexpr UICanvasDesc canvasDesc {
	.background = "gameartguppy/background.png",
};

} // namespace

// TODO LanguageScreen, AudioScreen

SettingsScreen::SettingsScreen(GameSettings& gameSettings)
    : mGameConfig(gameSettings)
    , mTitle(textDescs[0])
    , mGraphicsButton(MakeButton(graphicsButtonDesc, buttonBitmapDesc, textDescs[1]))
    , mAudioButton(MakeButton(audioButtonDesc, buttonBitmapDesc, textDescs[2]))
    , mLanguageButton(MakeButton(languageButtonDesc, buttonBitmapDesc, textDescs[3]))
    , mBackButton(MakeButton(backButtonDesc, buttonBitmapDesc, textDescs[4]))
    , mCanvas(canvasDesc) {
	// Build UI
	mCanvas.AddText(mTitle);
	mCanvas.AddButton(mGraphicsButton);
	mCanvas.AddButton(mLanguageButton);
	mCanvas.AddButton(mAudioButton);
	mCanvas.AddButton(mBackButton);
	RefreshLanguageButton();
}

const char* SettingsScreen::GetName() const {
	return "SettingsScreen";
}

void SettingsScreen::LoadAssets(Engine& engine) {
	mCanvas.LoadAssets(engine.GetGraphics(), engine.GetTextRenderer());
}

ScreenEvent SettingsScreen::Tick([[maybe_unused]] float dt, const Wind::Input& input) {
	if (mGraphicsButton.IsPressed(input)) {
		return { ScreenOp::goTo, GameScreenIds::graphicsSettings };
	}
	else if (mLanguageButton.IsPressed(input)) {
		SetNextLanguage();
	}
	else if (mAudioButton.IsPressed(input)) {
		return { ScreenOp::goTo, GameScreenIds::audioSettings };
	}

#if defined(__ANDROID__) || defined(__OHOS__)
	if (input.GetKeyJustPressed(SDLK_AC_BACK) ||
#elif defined(_WIN32) || defined(__linux__)
	if (input.GetKeyJustPressed(SDLK_ESCAPE) ||
#endif
	    mBackButton.IsPressed(input)) {
		return { ScreenOp::back };
	}
	return { ScreenOp::keep };
}

void SettingsScreen::Draw(Wind::UIRenderer& uiRenderer) {
	mCanvas.Draw(RefWindowWidth, RefWindowHeight, uiRenderer, 0);
}

void SettingsScreen::Enter([[maybe_unused]] ScreenId prevScreen, const void* payload) {
}

void SettingsScreen::Exit() {
}

void SettingsScreen::RefreshLanguageButton() {
	GameStringId stringId = GameStringId::nextLanguage;
	mLanguageButton.GetText()->SetText(static_cast<StringId>(stringId));
}

void SettingsScreen::ParseConfig(const char* varName, const char* varValue) {
}
