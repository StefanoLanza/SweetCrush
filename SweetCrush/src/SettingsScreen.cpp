#include "SettingsScreen.h"
#include "Config.h"
#include "GameSettings.h"
#include "Localization.h"
#include "ScreenIds.h"
#include "UIDefs.h"
#include <engine/Engine.h>
#include <engine/TextRender.h>
#include <engine/UI.h>

using namespace Wind;

namespace {

const UIButtonDesc buttonDescs[] { { { 0, 440, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top },
	                               { { 0, 560, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top },
	                               { { 0, 680, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top } };
const UITextDesc   textDescs[] {
    { "bigFont", (StringId)GameStringId::settings, { 0, titleY, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top, titleTextStyle },
    { "mediumFont", (StringId)GameStringId::nextLanguage, { 0, 0, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::center },
    { "mediumFont", (StringId)GameStringId::audioOn, { 0, 0, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::center },
    { "mediumFont", (StringId)GameStringId::back, { 0, 0, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::center },
};

} // namespace

SettingsScreen::SettingsScreen(Engine& engine, GameSettings& gameSettings)
    : mGameConfig(gameSettings)
    , mTitle(textDescs[0], engine)
    , mLanguageButton(MakeButton(buttonDescs[0], buttonBitmapDesc, textDescs[1], engine))
    , mAudioButton(MakeButton(buttonDescs[1], buttonBitmapDesc, textDescs[2], engine))
    , mBackButton(MakeButton(buttonDescs[2], buttonBitmapDesc, textDescs[3], engine))
    , mPanel(UIDefaultPanelDesc) {
}

void SettingsScreen::LoadAssets() {
}

void SettingsScreen::BuildUI(UICanvas& canvas) {
	mPanel.AddText(mTitle);
	mPanel.AddButton(mLanguageButton);
	mPanel.AddButton(mAudioButton);
	mPanel.AddButton(mBackButton);
	canvas.GetPanel().AddPanel(mPanel);
	RefreshLanguageButton();
	RefreshAudioButton();
}

GameScreenId SettingsScreen::Tick([[maybe_unused]] float dt, const Wind::Input& input) {
	if (mLanguageButton.IsPressed(input)) {
		SetNextLanguage();
	}
	else if (mAudioButton.IsPressed(input)) {
		if (mGameConfig.audioOn) {
			mGameConfig.audioOn = false;
		}
		else {
			mGameConfig.audioOn = true;
		}
		RefreshAudioButton();
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

void SettingsScreen::RefreshAudioButton() {
	const StringId stringId = static_cast<StringId>(mGameConfig.audioOn ? GameStringId::audioOn : GameStringId::audioOff);
	mAudioButton.GetText()->SetText(stringId);
}
