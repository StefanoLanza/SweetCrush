#include "AudioSettingsScreen.h"
#include "AppConfig.h"
#include "GameSettings.h"
#include "Localization.h"
#include "ScreenIds.h"
#include "UIDefs.h"

#include <engine/Engine.h>
#include <engine/Input.h>
#include <engine/UI.h>

using namespace Wind;

namespace {

const UIButtonDesc musicButtonDesc { { 0, 560, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top };
const UIButtonDesc sfxButtonDesc { { 0, 680, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top };
const UIButtonDesc backButtonDesc { { 0, 920, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top };

const UITextDesc titleTextDesc {
	.pos = { 0, titleY },
	.horizontalAlignment = UIHorizAlignment::center,
	.verticalAlignment = UIVertAlignment::top,
	.font = "screenTitle",
	.stringId = GameStringId::audioSettings,
};

const UITextDesc musicButtonTextDesc {
	.horizontalAlignment = UIHorizAlignment::center,
	.verticalAlignment = UIVertAlignment::center,
	.font = "mediumFont",
	.stringId = GameStringId::musicOn,
};

const UITextDesc sfxButtonTextDesc {
	.horizontalAlignment = UIHorizAlignment::center,
	.verticalAlignment = UIVertAlignment::center,
	.font = "mediumFont",
	.stringId = GameStringId::sfxOn,
};

constexpr UICanvasDesc canvasDesc {
	.background = "gameartguppy/background.png",
};

} // namespace

AudioSettingsScreen::AudioSettingsScreen(GameSettings& gameSettings)
    : mGameConfig(gameSettings)
    , mTitle(titleTextDesc, titleTextStyle)
    , mMusicButton(musicButtonDesc, buttonBitmapDesc, musicButtonTextDesc)
    , mSfxButton(sfxButtonDesc, buttonBitmapDesc, sfxButtonTextDesc)
    , mBackButton(defaultBackButtonDesc, defaultBackButtonBitmapDesc)
    , mCanvas(canvasDesc) {
	// Build UI
	mCanvas.AddText(mTitle);
	mCanvas.AddButton(mMusicButton);
	mCanvas.AddButton(mSfxButton);
	mCanvas.AddButton(mBackButton);
	RefreshMusicButton();
	RefreshSfxButton();
}

const char* AudioSettingsScreen::GetName() const {
	return "AudioSettingsScreen";
}

void AudioSettingsScreen::LoadAssets(Engine& engine) {
	mCanvas.LoadAssets(engine.GetGraphics(), engine.GetFontManager());
}

ScreenEvent AudioSettingsScreen::Tick([[maybe_unused]] float dt, const Wind::Input& input) {
	if (mMusicButton.IsClicked(input)) {
		mGameConfig.musicOn = ! mGameConfig.musicOn;
		RefreshMusicButton();
	}
	else if (mSfxButton.IsClicked(input)) {
		mGameConfig.sfxOn = ! mGameConfig.sfxOn;
		RefreshSfxButton();
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

void AudioSettingsScreen::Draw(Wind::UIRenderer& uiRenderer, float dt) {
	mCanvas.Draw(RefWindowWidth, RefWindowHeight, uiRenderer, 0);
}

void AudioSettingsScreen::Enter(const ScreenNavArgs& args) {
}

void AudioSettingsScreen::Exit() {
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
