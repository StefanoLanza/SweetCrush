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
    , mGraphicsButton { MakeMenuButton(button0_y, GameStringId::graphicsSettings) }
    , mMusicButton { MakeToggleButton(button1_y, GameStringId::music) }
    , mSfxButton { MakeToggleButton(button2_y, GameStringId::sfx) }
    , mLanguageButton { MakeMenuButton(button3_y, GameStringId::languageScreen) }
    , mBackButton { MakeBackButton() }
    , mCanvas(canvasDesc) {
	// Build UI
	mCanvas.AddText(mTitle);
	mCanvas.AddButton(mGraphicsButton);
	mCanvas.AddButton(mLanguageButton);
	mCanvas.AddButton(mMusicButton);
	mCanvas.AddButton(mSfxButton);
	mCanvas.AddButton(mBackButton);
}

const char* SettingsScreen::GetName() const {
	return "SettingsScreen";
}

void SettingsScreen::LoadAssets(Engine& engine) {
	mCanvas.LoadAssets(engine.GetGraphics(), engine.GetFontManager());
}

ScreenEvent SettingsScreen::Tick([[maybe_unused]] float dt, const Wind::Input& input) {
	mCanvas.HandleInput(input);

	mGameConfig.musicOn = mMusicButton.IsToggled();
	mGameConfig.sfxOn = mSfxButton.IsToggled();

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
	mMusicButton.SetToggled(mGameConfig.musicOn);
	mSfxButton.SetToggled(mGameConfig.sfxOn);
}

void SettingsScreen::Exit() {
}

void SettingsScreen::ParseConfig(const char* varName, const char* varValue) {
}

void SettingsScreen::RefreshLanguageButton() {
	GameStringId stringId = GameStringId::nextLanguage;
	mLanguageButton.GetText()->SetText(static_cast<StringId>(stringId));
}
