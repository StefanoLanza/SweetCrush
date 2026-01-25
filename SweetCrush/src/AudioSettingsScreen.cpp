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
	.pos = { 0, titleY, 0, 0 },
	.size = UIAutoSize,
	.horizontalAlignment = UIHorizAlignment::center,
	.verticalAlignment = UIVertAlignment::top,
	.font = "bigFont",
	.stringId = (StringId)GameStringId::audioSettings,
	.textStyle = titleTextStyle,
};

const UITextDesc musicButtonTextDesc {

	.pos = { 0, 0, 0, 0 },
	.size = UIAutoSize,
	.horizontalAlignment = UIHorizAlignment::center,
	.verticalAlignment = UIVertAlignment::center,
	.font = "mediumFont",
	.stringId = (StringId)GameStringId::musicOn,
};

const UITextDesc sfxButtonTextDesc {
	.pos = { 0, 0, 0, 0 },
	.size = UIAutoSize,
	.horizontalAlignment = UIHorizAlignment::center,
	.verticalAlignment = UIVertAlignment::center,
	.font = "mediumFont",
	.stringId = (StringId)GameStringId::sfxOn,
};

const UITextDesc backTextDesc {
	.pos = { 0, 0, 0, 0 },
	.size = UIAutoSize,
	.horizontalAlignment = UIHorizAlignment::center,
	.verticalAlignment = UIVertAlignment::center,
	.font = "mediumFont",
	.stringId = (StringId)GameStringId::back,
};

constexpr UICanvasDesc canvasDesc {
	.background = "gameartguppy/background.png",
};

} // namespace

AudioSettingsScreen::AudioSettingsScreen(Engine& engine, GameSettings& gameSettings)
    : mGameConfig(gameSettings)
    , mTitle(titleTextDesc, engine.GetTextRenderer())
    , mMusicButton(MakeButton(musicButtonDesc, buttonBitmapDesc, musicButtonTextDesc, engine))
    , mSfxButton(MakeButton(sfxButtonDesc, buttonBitmapDesc, sfxButtonTextDesc, engine))
    , mBackButton(MakeButton(backButtonDesc, buttonBitmapDesc, backTextDesc, engine))
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
	mCanvas.LoadGraphics(engine.GetGraphics());
}

ScreenTransition AudioSettingsScreen::Tick([[maybe_unused]] float dt, const Wind::Input& input) {
	if (mMusicButton.IsPressed(input)) {
		mGameConfig.musicOn = ! mGameConfig.musicOn;
		RefreshMusicButton();
	}
	else if (mSfxButton.IsPressed(input)) {
		mGameConfig.sfxOn = ! mGameConfig.sfxOn;
		RefreshSfxButton();
	}

#if defined(__ANDROID__) || defined(__OHOS__)
	if (input.GetKeyJustPressed(SDLK_AC_BACK) ||
#elif defined(_WIN32) || defined(__linux__)
	if (input.GetKeyJustPressed(SDLK_ESCAPE) ||
#endif
	    mBackButton.IsPressed(input)) {
		return { ScreenOp::replace, GameScreenIds::settings };
	}
	return { ScreenOp::keep };
}

void AudioSettingsScreen::Draw(Wind::UIRenderer& uiRenderer) {
	mCanvas.Draw(RefWindowWidth, RefWindowHeight, uiRenderer, 0);
}

void AudioSettingsScreen::Enter([[maybe_unused]] ScreenId prevScreen, const void* payload) {
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
