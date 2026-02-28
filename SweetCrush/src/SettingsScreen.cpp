#include "SettingsScreen.h"
#include "AppConfig.h"
#include "GameUI.h"
#include "Localization.h"
#include "ScreenIds.h"

#include <engine/Audio.h>
#include <engine/Engine.h>
#include <engine/Input.h>
#include <engine/SdlSound.h>

using namespace Wind;

SettingsScreen::SettingsScreen(GameSettings& gameSettings)
    : mGameConfig(gameSettings)
    , mCanvas(MakeCanvas()) {
	// Build UI
	mCanvas.Add(MakeTitle(GameStringId::settings));
	mMusicButton = mCanvas.Add(MakeCheckBox(button0_y, GameStringId::music, button0_color));
	mSfxButton = mCanvas.Add(MakeCheckBox(button1_y, GameStringId::sfx, button1_color));
	mBackButton = mCanvas.Add(MakeBackButton());
	MakeLanguageButton();
}

const char* SettingsScreen::GetName() const {
	return "SettingsScreen";
}

void SettingsScreen::LoadAssets(Engine& engine) {
	mCanvas.LoadAssets(engine.GetGraphics(), engine.GetFontManager());
	mButtonSound = engine.GetAudio().LoadSound("audio/click_001.ogg");
	mToggleSound = engine.GetAudio().LoadSound("audio/click_001.ogg");
}

ScreenEvent SettingsScreen::Tick([[maybe_unused]] float dt, const Wind::Input& input) {
	mCanvas.Tick(dt);
	mCanvas.HandleInput(input);

	bool dirtyUI = false;
	if (mMusicButton->IsClicked()) {
		mGameConfig.musicOn = ! mGameConfig.musicOn;
		if (mGameConfig.sfxOn) {
			mToggleSound->Play();
		}
		dirtyUI = true;
	}
	if (mSfxButton->IsClicked()) {
		mGameConfig.sfxOn = ! mGameConfig.sfxOn;
		if (mGameConfig.sfxOn) {
			mToggleSound->Play();
		}
		dirtyUI = true;
	}
	if (mLanguageButton->IsClicked()) {
		SetNextLanguage();
		if (mGameConfig.sfxOn) {
			mToggleSound->Play();
		}
		dirtyUI = true;
	}
	if (dirtyUI) {
		RefreshUI();
	}

#if defined(__ANDROID__) || defined(__OHOS__)
	if (input.GetKeyJustPressed(SDLK_AC_BACK) ||
#elif defined(_WIN32) || defined(__linux__)
	if (input.GetKeyJustPressed(SDLK_ESCAPE) ||
#endif
	    mBackButton->IsClicked()) {
		if (mGameConfig.sfxOn) {
			mButtonSound->Play();
		}
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
	mMusicButton->GetBitmap(0).SetVisible(musicOn);
	mMusicButton->GetBitmap(1).SetVisible(! musicOn);
	mSfxButton->GetBitmap(0).SetVisible(sfxOn);
	mSfxButton->GetBitmap(1).SetVisible(! sfxOn);

	for (int i = 0; i < 3; ++i) {
		UIBitmap& icon = mLanguageButton->GetBitmap(i);
		icon.SetVisible(static_cast<int>(GetCurrentLanguage()) == i);
	}
}

void SettingsScreen::MakeLanguageButton() {
	const UIButtonDesc buttonDesc = {
		.pos = UIAbsolutePos(0, button2_y),
		.size = UIAbsoluteSize(520.f, 100.f),
		.horizontalAlignment = UIHorizAlignment::center,
		.verticalAlignment = UIVertAlignment::top,
		.padding = buttonPadding,
		.background = "UI/button.png",
		.backgroundColor = button2_color,
		._9patch = 16.f,
	};
	const UITextDesc labelDesc {
		.stringId = GameStringId::languageScreen,
		.horizontalAlignment = UIHorizAlignment::left,
		.verticalAlignment = UIVertAlignment::center,
		.font = "mediumFont",
		.style = defaultTextStyle,
	};

	mLanguageButton = mCanvas.Add(buttonDesc);

	UIBitmapDesc iconDesc {
		.pos = UIAbsolutePos(16.f, 0.f),
		.horizontalAlignment = UIHorizAlignment::right,
		.verticalAlignment = UIVertAlignment::center,
		.sizing = UIBitmapSizing::fit,
	};
	const char* languageIcons[] {
		"icons/uk.png",
		"icons/spain.png",
		"icons/italy.png",
	};
	for (int i = 0; i < 3; ++i) {
		iconDesc.fileName = languageIcons[i];
		mLanguageButton->Add(iconDesc);
	}

	mLanguageButton->Add(labelDesc);
}
