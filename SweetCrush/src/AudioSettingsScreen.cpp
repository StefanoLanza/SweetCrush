#include "AudioSettingsScreen.h"
#include "AppConfig.h"
#include "GameSettings.h"
#include "Localization.h"
#include "ScreenIds.h"
#include "UIDefs.h"
#include <engine/Engine.h>
#include <engine/UI.h>

using namespace Wind;

namespace {

const UIButtonDesc musicButtonDesc { { 0, 560, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top };
const UIButtonDesc sfxButtonDesc { { 0, 680, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top };
const UIButtonDesc backButtonDesc { { 0, 920, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top };

const UITextDesc titleTextDesc {
	"bigFont", (StringId)GameStringId::audioSettings, { 0, titleY, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top, titleTextStyle
};

const UITextDesc musicButtonTextDesc { "mediumFont", (StringId)GameStringId::musicOn, { 0, 0, 0, 0 },
	                                   UIAutoSize,   UIHorizAlignment::center,        UIVertAlignment::center };
const UITextDesc sfxButtonTextDesc { "mediumFont", (StringId)GameStringId::sfxOn, { 0, 0, 0, 0 },
	                                 UIAutoSize,   UIHorizAlignment::center,      UIVertAlignment::center };
const UITextDesc backTextDesc { "mediumFont", (StringId)GameStringId::back, { 0, 0, 0, 0 },
	                            UIAutoSize,   UIHorizAlignment::center,     UIVertAlignment::center };

} // namespace

AudioSettingsScreen::AudioSettingsScreen(Engine& engine, GameSettings& gameSettings)
    : mGameConfig(gameSettings)
    , mTitle(titleTextDesc, engine)
    , mMusicButton(MakeButton(musicButtonDesc, buttonBitmapDesc, musicButtonTextDesc, engine))
    , mSfxButton(MakeButton(sfxButtonDesc, buttonBitmapDesc, sfxButtonTextDesc, engine))
    , mBackButton(MakeButton(backButtonDesc, buttonBitmapDesc, backTextDesc, engine))
    , mPanel(UIDefaultPanelDesc) {
}

const char* AudioSettingsScreen::GetName() const {
	return "AudioSettingsScreen";
}

void AudioSettingsScreen::LoadAssets() {
}

void AudioSettingsScreen::BuildUI(UICanvas& canvas) {
	mPanel.AddText(mTitle);
	mPanel.AddButton(mMusicButton);
	mPanel.AddButton(mSfxButton);
	mPanel.AddButton(mBackButton);
	canvas.GetPanel().AddPanel(mPanel);
	RefreshMusicButton();
	RefreshSfxButton();
}

GameScreenId AudioSettingsScreen::Tick([[maybe_unused]] float dt, const Wind::Input& input) {
	if (mMusicButton.IsPressed(input)) {
		mGameConfig.musicOn = ! mGameConfig.musicOn;
		RefreshMusicButton();
	}
	else if (mSfxButton.IsPressed(input)) {
		mGameConfig.sfxOn = ! mGameConfig.sfxOn;
		RefreshSfxButton();
	}
	else if (mBackButton.IsPressed(input)) {
		return ScreenId::settings;
	}
	return ScreenId::audioSettings;
}

void AudioSettingsScreen::Draw([[maybe_unused]] GameScreenId topScreen) const {
}

void AudioSettingsScreen::Enter([[maybe_unused]] GameScreenId prevScreen) {
	mPanel.SetVisible(true);
}

void AudioSettingsScreen::Exit() {
	mPanel.SetVisible(false);
}

void AudioSettingsScreen::RefreshMusicButton() {
	const StringId stringId = static_cast<StringId>(mGameConfig.musicOn ? GameStringId::musicOn : GameStringId::musicOff);
	mMusicButton.GetText()->SetText(stringId);
}

void AudioSettingsScreen::RefreshSfxButton() {
	const StringId stringId = static_cast<StringId>(mGameConfig.sfxOn ? GameStringId::sfxOn : GameStringId::sfxOff);
	mSfxButton.GetText()->SetText(stringId);
}

void AudioSettingsScreen::ParseConfig(const char* varName, const char* varValue) {
}
