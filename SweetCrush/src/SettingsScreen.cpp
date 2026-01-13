#include "SettingsScreen.h"
#include "GameConfig.h"
#include "GameSettings.h"
#include "Localization.h"
#include "ScreenIds.h"
#include "UIDefs.h"
#include <engine/Engine.h>
#include <engine/TextRender.h>
#include <engine/UI.h>

using namespace Wind;

namespace {

const UIButtonDesc graphicsButtonDesc { { 0, 440, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top };

const UIButtonDesc buttonDescs[] {
	{ { 0, 560, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top },
	{ { 0, 680, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top },
	{ { 0, 800, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top },
	{ { 0, 920, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top },
};
const UITextDesc textDescs[] {
	{ "bigFont", (StringId)GameStringId::settings, { 0, titleY, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top, titleTextStyle },
	{ "mediumFont", (StringId)GameStringId::graphicsSettings, { 0, 0, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::center },
	{ "mediumFont", (StringId)GameStringId::nextLanguage, { 0, 0, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::center },
	{ "mediumFont", (StringId)GameStringId::musicOn, { 0, 0, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::center },
	{ "mediumFont", (StringId)GameStringId::sfxOn, { 0, 0, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::center },
	{ "mediumFont", (StringId)GameStringId::back, { 0, 0, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::center },
};

} // namespace

// TODO LanguageScreen, AudioScreen

SettingsScreen::SettingsScreen(Engine& engine, GameSettings& gameSettings)
    : mGameConfig(gameSettings)
    , mTitle(textDescs[0], engine)
    , mGraphicsButton(MakeButton(graphicsButtonDesc, buttonBitmapDesc, textDescs[1], engine))
    , mLanguageButton(MakeButton(buttonDescs[0], buttonBitmapDesc, textDescs[2], engine))
    , mMusicButton(MakeButton(buttonDescs[1], buttonBitmapDesc, textDescs[3], engine))
    , mSfxButton(MakeButton(buttonDescs[2], buttonBitmapDesc, textDescs[4], engine))
    , mBackButton(MakeButton(buttonDescs[3], buttonBitmapDesc, textDescs[5], engine))
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
	mPanel.AddButton(mMusicButton);
	mPanel.AddButton(mSfxButton);
	mPanel.AddButton(mBackButton);
	canvas.GetPanel().AddPanel(mPanel);
	RefreshLanguageButton();
	RefreshMusicButton();
	RefreshSfxButton();
}

GameScreenId SettingsScreen::Tick([[maybe_unused]] float dt, const Wind::Input& input) {
	if (mGraphicsButton.IsPressed(input)) {
		return ScreenId::graphicsSettings;
	}
	else if (mLanguageButton.IsPressed(input)) {
		SetNextLanguage();
	}
	else if (mMusicButton.IsPressed(input)) {
		mGameConfig.musicOn = ! mGameConfig.musicOn;
		RefreshMusicButton();
	}
	else if (mSfxButton.IsPressed(input)) {
		mGameConfig.sfxOn = ! mGameConfig.sfxOn;
		RefreshSfxButton();
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

void SettingsScreen::RefreshMusicButton() {
	const StringId stringId = static_cast<StringId>(mGameConfig.musicOn ? GameStringId::musicOn : GameStringId::musicOff);
	mMusicButton.GetText()->SetText(stringId);
}

void SettingsScreen::RefreshSfxButton() {
	const StringId stringId = static_cast<StringId>(mGameConfig.sfxOn ? GameStringId::sfxOn : GameStringId::sfxOff);
	mSfxButton.GetText()->SetText(stringId);
}

void SettingsScreen::ParseConfig(const char* varName, const char* varValue) {
}
