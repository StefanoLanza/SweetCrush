#include "SettingsScreen.h"
#include "AppConfig.h"
#include "GameUI.h"
#include "Localization.h"
#include "ScreenIds.h"

#include <engine/Engine.h>
#include <engine/Input.h>

using namespace Wind;

SettingsScreen::SettingsScreen(GameSettings& gameSettings)
    : mGameConfig(gameSettings)
    , mMusicButton { MakeCheckBox(button0_y, GameStringId::music, button0_color) }
    , mSfxButton { MakeCheckBox(button1_y, GameStringId::sfx, button1_color) }
    , mLanguageButton { MakeMenuButton(button2_y, GameStringId::languageScreen, button2_color) }
    , mBackButton { MakeBackButton() }
    , mCanvas(MakeCanvas()) {
	// Build UI
	mCanvas.Add(GetTitleTextDesc(GameStringId::settings));
	mCanvas.Add(mLanguageButton);
	mCanvas.Add(mMusicButton);
	mCanvas.Add(mSfxButton);
	mCanvas.Add(mBackButton);
}

const char* SettingsScreen::GetName() const {
	return "SettingsScreen";
}

void SettingsScreen::LoadAssets(Engine& engine) {
	mCanvas.LoadAssets(engine.GetGraphics(), engine.GetFontManager());
}

ScreenEvent SettingsScreen::Tick([[maybe_unused]] float dt, const Wind::Input& input) {
	mCanvas.Tick(dt);
	mCanvas.HandleInput(input);

	if (mMusicButton.IsClicked()) {
		mGameConfig.musicOn = ! mGameConfig.musicOn;
	}
	if (mSfxButton.IsClicked()) {
		mGameConfig.sfxOn = ! mGameConfig.sfxOn;
	}
	RefreshUI();

	if (mLanguageButton.IsClicked()) {
		SetNextLanguage();
	}

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

void SettingsScreen::Draw(Wind::UIRenderer& uiRenderer, float dt) {
	mCanvas.Draw(RefWindowWidth, RefWindowHeight, uiRenderer, 0);
}

void SettingsScreen::Enter(const ScreenNavArgs& args) {
	RefreshUI();
}

void SettingsScreen::Exit() {
}

void SettingsScreen::ParseConfig(const char* varName, const char* varValue) {
}

void SettingsScreen::RefreshUI() {
	bool musicOn = mGameConfig.musicOn;
	bool sfxOn = mGameConfig.sfxOn;
	mMusicButton.GetBitmap(0).SetVisible(musicOn);
	mMusicButton.GetBitmap(1).SetVisible(! musicOn);
	mSfxButton.GetBitmap(0).SetVisible(sfxOn);
	mSfxButton.GetBitmap(1).SetVisible(! sfxOn);
}