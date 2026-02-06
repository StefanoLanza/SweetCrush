#include "AudioSettingsScreen.h"
#include "AppConfig.h"
#include "GameSettings.h"
#include "GameUI.h"
#include "Localization.h"
#include "ScreenIds.h"

#include <engine/Engine.h>
#include <engine/Input.h>
#include <engine/UI.h>

using namespace Wind;

namespace {

const UIButtonDesc backButtonDesc { { 0, 920, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top };

const UITextDesc titleTextDesc {
	.pos = { 0, titleY },
	.horizontalAlignment = UIHorizAlignment::center,
	.verticalAlignment = UIVertAlignment::top,
	.font = "screenTitle",
	.stringId = GameStringId::audioSettings,
};

constexpr UICanvasDesc canvasDesc {
	.background = "gameartguppy/background.png",
};

} // namespace

AudioSettingsScreen::AudioSettingsScreen(GameSettings& gameSettings)
    : mGameConfig(gameSettings)
    , mTitle(titleTextDesc, titleTextStyle)
    , mMusicButton { MakeToggleButton(560.f, GameStringId::music) }
    , mSfxButton { MakeToggleButton(680.f, GameStringId::sfx) }
    , mBackButton(defaultBackButtonDesc, defaultBackButtonBitmapDesc)
    , mCanvas(canvasDesc) {
	// Build UI
	mCanvas.AddText(mTitle);
	mCanvas.AddButton(mMusicButton);
	mCanvas.AddButton(mSfxButton);
	mCanvas.AddButton(mBackButton);
	// RefreshMusicButton();
	// RefreshSfxButton();
}

const char* AudioSettingsScreen::GetName() const {
	return "AudioSettingsScreen";
}

void AudioSettingsScreen::LoadAssets(Engine& engine) {
	mCanvas.LoadAssets(engine.GetGraphics(), engine.GetFontManager());
}

ScreenEvent AudioSettingsScreen::Tick([[maybe_unused]] float dt, const Wind::Input& input) {
	mCanvas.HandleInput(input);
	mGameConfig.musicOn = mMusicButton.IsToggled();
	mGameConfig.sfxOn = mSfxButton.IsToggled();
	// RefreshMusicButton();
	// RefreshSfxButton();

#if defined(__ANDROID__) || defined(__OHOS__)
	if (input.GetKeyJustPressed(SDLK_AC_BACK) ||
#elif defined(_WIN32) || defined(__linux__)
	if (input.GetKeyJustPressed(SDLK_ESCAPE) ||
#endif
	    mBackButton.IsClicked()) {
		return GoBack(ScreenTransition::slideRight);
	}
	return Continue();
}

void AudioSettingsScreen::Draw(Wind::UIRenderer& uiRenderer, float dt) {
	mCanvas.Draw(RefWindowWidth, RefWindowHeight, uiRenderer, 0);
}

void AudioSettingsScreen::Enter(const ScreenNavArgs& args) {
	mMusicButton.SetToggled(mGameConfig.musicOn);
	mSfxButton.SetToggled(mGameConfig.sfxOn);
}

void AudioSettingsScreen::Exit() {
}

void AudioSettingsScreen::ParseConfig(const char* varName, const char* varValue) {
}
