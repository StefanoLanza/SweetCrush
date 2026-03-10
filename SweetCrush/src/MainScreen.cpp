#include "MainScreen.h"
#include "AssetDefs.h"
#include "Constants.h"
#include "GameSettings.h"
#include "GameUI.h"
#include "Localization.h"
#include "ScreenIds.h"

#include <engine/Audio.h>
#include <engine/Engine.h>
#include <engine/Input.h>
#include <engine/SdlSound.h>

using namespace Wind;

namespace {

const UITextDesc versionDesc {
	.stringId = GameStringId::version,
	.pos = { 0.f, 0.f },
	.horizontalAlignment = UIHorizAlignment::right,
	.verticalAlignment = UIVertAlignment::bottom,
	.font = "smallFont",
	.style = defaultTextStyle,
};

const UIPanelDesc pastryPanelDesc {
	.pos = UIAbsolutePos(0.f, 250.f),
	.size = { 0.f, 96.f, 1.f, 0.f },
	.horizontalAlignment = UIHorizAlignment::center,
	.verticalAlignment = UIVertAlignment::top,
	.backgroundColor = transparentColor,
	.grid {
	    .cols = 9,
	},
};

} // namespace

MainScreen::MainScreen(Engine& engine, const GameSettings& gameSettings)
    : mEngine(engine)
    , mGameSettings(gameSettings)
    , mCanvas { MakeCanvas() }
    , mAccumTime(0) {
	// Setup UI
	mCanvas.Add(MakeTitle(GameStringId::title));
	mStartButton = MakeMenuButton(mCanvas.Panel(), button0_y, GameStringId::start, button0_color);       //, "icons/play.png"));
	mSettingsButton = MakeMenuButton(mCanvas.Panel(), button1_y, GameStringId::settings, button1_color); //, "icons/gear.png"));
	mCreditsButton = MakeMenuButton(mCanvas.Panel(), button2_y, GameStringId::credits, button2_color);   //, "icons/info.png"));
#if ! defined(__ANDROID__) && ! defined(__OHOS__)
	mQuitButton = MakeMenuButton(mCanvas.Panel(), button3_y, GameStringId::quit, button3_color); //, "icons/cross.png"));
#endif
	mCanvas.Add(versionDesc);

	UISliderDesc sliderDesc {
		.pos = UIAbsolutePos(0.f, 100.f),
		.size = { 0.f, 96.f, 1.f, 0.f },
		.horizontalAlignment = UIHorizAlignment::center,
		.verticalAlignment = UIVertAlignment::top,
		.backgroundColor = yellowColor,
	};
	mSlider = mCanvas.Panel().Add(sliderDesc);
	mSlider->SetValue(0.3f);
	{
		UISliderThumbDesc iconDesc;
		iconDesc.fileName = gameTexturePath[0];
		iconDesc.size = { 64.f, 64.f };
		mSlider->SetThumb(iconDesc);
	}

	mPastryPanel = mCanvas.Add(pastryPanelDesc);
	for (int i = 0; i < NumPieceTypes; ++i) {
		UIBitmapDesc iconDesc;
		iconDesc.horizontalAlignment = UIHorizAlignment::center;
		iconDesc.verticalAlignment = UIVertAlignment::center;
		iconDesc.fileName = gameTexturePath[i];
		iconDesc.sizing = UIBitmapSizing::fit;
		iconDesc.pivot = { 0.5f, 0.5f };
		mPastryPanel->Add(iconDesc);
	}
}

const char* MainScreen::GetName() const {
	return "MainScreen";
}

void MainScreen::LoadAssets(Engine& engine) {
	mCanvas.LoadAssets(engine.GetGraphics(), engine.GetFontManager());
	mButtonSound = engine.GetAudio().LoadSound("audio/click_001.ogg");
}

ScreenEvent MainScreen::Tick(float dt, const Wind::Input& input) {
	mCanvas.Tick(dt);
	mCanvas.HandleInput(input);
	mAccumTime += dt;

	if (mStartButton->IsClicked()) {
		if (mGameSettings.sfxOn) {
			mButtonSound->Play();
		}
		return GoTo(GameScreenIds::levelStart, ScreenTransition::slideTop);
	}
	else if (mSettingsButton->IsClicked()) {
		if (mGameSettings.sfxOn) {
			mButtonSound->Play();
		}
		return GoTo(GameScreenIds::settings, ScreenTransition::slideLeft);
	}
	else if (mCreditsButton->IsClicked()) {
		if (mGameSettings.sfxOn) {
			mButtonSound->Play();
		}
		return GoTo(GameScreenIds::credits, ScreenTransition::slideLeft);
	}

	AnimateUI();

#if defined(__ANDROID__) || defined(__OHOS__)
	if (input.GetKeyJustPressed(SDLK_AC_BACK)) {
		mEngine.Quit();
#elif defined(_WIN32) || defined(__linux__)
	if (input.GetKeyJustPressed(SDLK_ESCAPE) || mQuitButton->IsClicked()) {
#endif
		mEngine.Quit();
	}
	return Continue();
}

void MainScreen::Draw(UIRenderer& uiRenderer, float dt) {
	mCanvas.Draw(RefWindowWidth, RefWindowHeight, uiRenderer, 0);

	// Rotate and oscillate pastry icons
	float phase = mAccumTime * 4.f;
	for (int i = 0; i < NumPieceTypes; ++i) {
		UITransform& iconTransform = mPastryPanel->GetControl(i).GetTransform();
		iconTransform.rotation = std::sin(phase * .25f + (float)i) * 0.5f;
		iconTransform.offset.y = std::cos(phase) * 4.f;
		phase += 6.28f / static_cast<float>(NumPieceTypes);
	}
}

void MainScreen::Enter(const ScreenNavArgs& args) {
	mAccumTime = 0.f;
	AnimateUI();
}

void MainScreen::Exit() {
}

void MainScreen::ParseConfig(const char* varName, const char* varValue) {
}

void MainScreen::AnimateUI() {
	//	float t = std::min(1.f, mAccumTime * 3.f);
	//	auto  desc = mSettingsButton.GetDesc();
	//	mQuitButton.GetDesc Bitmap()->SetColor(Color { desc.color.r, desc.color.g, desc.color.b, 255.f * t });
	// desc.scale = LerpEase(0.85f, 1.f, t, EaseOutBounce);
	// desc.pos.ax = LerpEase(-400.f, 0.f, t, EaseOutCubic);
	// mSettingsButton.SetDesc(desc);
	// desc = mStartButton.GetDesc();
	// desc.pos.ax = LerpEase(400.f, 0.f, t, EaseOutCubic);
	// mStartButton.SetDesc(desc);
}
