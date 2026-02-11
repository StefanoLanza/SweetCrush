#include "SettingsScreen.h"
#include "AppConfig.h"
#include "GameUI.h"
#include "Localization.h"
#include "ScreenIds.h"

#include <engine/Engine.h>
#include <engine/Input.h>
#include <engine/UI.h>

using namespace Wind;

namespace {

constexpr UICanvasDesc canvasDesc {
	.background = "gameartguppy/background.png",
};

} // namespace

SettingsScreen::SettingsScreen(GameSettings& gameSettings)
    : mGameConfig(gameSettings)
    , mTitle { MakeTitleText(GameStringId::settings) }
    , mMusicButton { MakeCheckBox(button0_y, GameStringId::music, button0_color) }
    , mSfxButton { MakeCheckBox(button1_y, GameStringId::sfx, button1_color) }
    , mLanguageButton { MakeMenuButton(button2_y, GameStringId::languageScreen, button2_color) }
    , mBackButton { MakeBackButton() }
    , mCanvas(canvasDesc) {
	// Build UI
	mCanvas.Add(mTitle);
	mCanvas.Add(mLanguageButton);
	// TODO mCanvas.Add(mMusicButton);
	// TODO mCanvas.Add(mSfxButton);
	mCanvas.Add(mBackButton);
}

const char* SettingsScreen::GetName() const {
	return "SettingsScreen";
}

void SettingsScreen::LoadAssets(Engine& engine) {
	mCanvas.LoadAssets(engine.GetGraphics(), engine.GetFontManager());
}

ScreenEvent SettingsScreen::Tick([[maybe_unused]] float dt, const Wind::Input& input) {
	mCanvas.HandleInput(input);

	mGameConfig.musicOn = mMusicButton.IsChecked();
	mGameConfig.sfxOn = mSfxButton.IsChecked();

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
	mMusicButton.SetChecked(mGameConfig.musicOn);
	mSfxButton.SetChecked(mGameConfig.sfxOn);
}

void SettingsScreen::Exit() {
}

void SettingsScreen::ParseConfig(const char* varName, const char* varValue) {
}

void SettingsScreen::RefreshLanguageButton() {
	GameStringId stringId = GameStringId::nextLanguage;
	//TODO mLanguageButton.GetText()->SetText(static_cast<StringId>(stringId));
}
