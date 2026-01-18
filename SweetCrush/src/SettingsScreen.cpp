#include "SettingsScreen.h"
#include "AppConfig.h"
#include "Localization.h"
#include "ScreenIds.h"
#include "UIDefs.h"
#include <engine/Engine.h>
#include <engine/UI.h>

using namespace Wind;

namespace {

const UIButtonDesc graphicsButtonDesc { { 0, 560, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top };
const UIButtonDesc audioButtonDesc { { 0, 680, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top };
const UIButtonDesc languageButtonDesc { { 0, 800, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top };
const UIButtonDesc backButtonDesc { { 0, 920, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top };

const UITextDesc textDescs[] {
	{ "bigFont", (StringId)GameStringId::settings, { 0, titleY, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top, titleTextStyle },
	{ "mediumFont", (StringId)GameStringId::graphicsSettings, { 0, 0, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::center },
	{ "mediumFont", (StringId)GameStringId::audioSettings, { 0, 0, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::center },
	{ "mediumFont", (StringId)GameStringId::nextLanguage, { 0, 0, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::center },
	{ "mediumFont", (StringId)GameStringId::back, { 0, 0, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::center },
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

GameScreenId SettingsScreen::Tick([[maybe_unused]] float dt, const Wind::Input& input) {
	if (mGraphicsButton.IsPressed(input)) {
		return ScreenId::graphicsSettings;
	}
	else if (mLanguageButton.IsPressed(input)) {
		SetNextLanguage();
	}
	else if (mAudioButton.IsPressed(input)) {
		return ScreenId::audioSettings;
	}
	else if (mBackButton.IsPressed(input)) {
		return ScreenId::mainMenu;
	}
	return ScreenId::settings;
}

void SettingsScreen::Draw([[maybe_unused]] GameScreenId topScreen) const {
}

void SettingsScreen::Enter([[maybe_unused]] GameScreenId prevScreen) {
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
