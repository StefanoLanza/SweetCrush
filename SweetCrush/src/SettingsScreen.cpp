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

} // namespace

// TODO LanguageScreen, AudioScreen

SettingsScreen::SettingsScreen(Engine& engine, GameSettings& gameSettings)
    : mGameConfig(gameSettings)
    , mTitle(textDescs[0], engine)
    , mGraphicsButton(MakeButton(graphicsButtonDesc, buttonBitmapDesc, textDescs[1], engine))
    , mAudioButton(MakeButton(audioButtonDesc, buttonBitmapDesc, textDescs[2], engine))
    , mLanguageButton(MakeButton(languageButtonDesc, buttonBitmapDesc, textDescs[3], engine))
    , mBackButton(MakeButton(backButtonDesc, buttonBitmapDesc, textDescs[4], engine))
    , mPanel(UIDefaultPanelDesc) {
}

const char* SettingsScreen::GetName() const {
	return "SettingsScreen";
}

void SettingsScreen::LoadAssets() {
}

void SettingsScreen::BuildUI(UICanvas& canvas) {
	mPanel.AddText(mTitle);
	mPanel.AddButton(mGraphicsButton);
	mPanel.AddButton(mLanguageButton);
	mPanel.AddButton(mAudioButton);
	mPanel.AddButton(mBackButton);
	canvas.GetPanel().AddPanel(mPanel);
	RefreshLanguageButton();
}

ScreenTransition SettingsScreen::Tick([[maybe_unused]] float dt, const Wind::Input& input) {
	if (mGraphicsButton.IsPressed(input)) {
		return { ScreenOp::replace, GameScreenIds::graphicsSettings };
	}
	else if (mLanguageButton.IsPressed(input)) {
		SetNextLanguage();
	}
	else if (mAudioButton.IsPressed(input)) {
		return { ScreenOp::replace, GameScreenIds::audioSettings };
	}

#if defined(__ANDROID__) || defined(__OHOS__)
	if (input.GetKeyJustPressed(SDLK_AC_BACK) ||
#elif defined(_WIN32) || defined(__linux__)
	if (input.GetKeyJustPressed(SDLK_ESCAPE) ||
#endif
	    mBackButton.IsPressed(input)) {
		return { ScreenOp::replace, GameScreenIds::mainMenu };
	}
	return { ScreenOp::keep };
}

void SettingsScreen::Draw([[maybe_unused]] ScreenId topScreen) const {
}

void SettingsScreen::Enter([[maybe_unused]] ScreenId prevScreen, const void* payload) {
	mPanel.SetVisible(true);
}

void SettingsScreen::Exit() {
	mPanel.SetVisible(false);
}

void SettingsScreen::RefreshLanguageButton() {
	GameStringId stringId = GameStringId::nextLanguage;
	mLanguageButton.GetText()->SetText(static_cast<StringId>(stringId));
}

void SettingsScreen::ParseConfig(const char* varName, const char* varValue) {
}
