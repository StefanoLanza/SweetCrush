#include "MainScreen.h"
#include "AssetDefs.h"
#include "Constants.h"
#include "GameDrawOrder.h"
#include "Localization.h"
#include "ScreenIds.h"
#include "UIDefs.h"
#include <cmath>
#include <engine/BitmapRender.h>
#include <engine/Engine.h>
#include <engine/TextRender.h>
#include <engine/UI.h>

using namespace Wind;

namespace {

const UIButtonDesc buttonDescs[4] {
	{ UIAbsolutePos(0, 440), UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top },
	{ UIAbsolutePos(0, 560), UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top },
	{ UIAbsolutePos(0, 680), UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top },
	{ UIAbsolutePos(0, 800), UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top },
};
const UITextDesc textDescs[5] {
	{ "bigFont", (StringId)GameStringId::title, UIAbsolutePos(0, titleY), UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top,
	  titleTextStyle },
	{ "mediumFont", (StringId)GameStringId::start, UIZeroPos, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::center },
	{ "mediumFont", (StringId)GameStringId::settings, UIZeroPos, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::center },
	{ "mediumFont", (StringId)GameStringId::credits, UIZeroPos, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::center },
	{ "mediumFont", (StringId)GameStringId::quit, UIZeroPos, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::center },
};

} // namespace

MainScreen::MainScreen(Engine& engine)
    : mEngine(engine)
    , mTitle(textDescs[0], engine)
    , mStartButton(MakeButton(buttonDescs[0], buttonBitmapDesc, textDescs[1], engine))
    , mSettingsButton(MakeButton(buttonDescs[1], buttonBitmapDesc, textDescs[2], engine))
    , mCreditsButton(MakeButton(buttonDescs[2], buttonBitmapDesc, textDescs[3], engine))
    , mQuitButton(MakeButton(buttonDescs[3], buttonBitmapDesc, textDescs[4], engine))
    , mPanel(UIDefaultPanelDesc)
    , mTime(0) {
}

void MainScreen::LoadAssets() {
}

void MainScreen::BuildUI(UICanvas& canvas) {
	mPanel.AddText(mTitle);
	mPanel.AddButton(mStartButton);
	mPanel.AddButton(mSettingsButton);
	mPanel.AddButton(mCreditsButton);
#if ! defined(__ANDROID__) && (defined(_WIN32) || defined(__linux__))
	mPanel.AddButton(mQuitButton);
#endif
	canvas.GetPanel().AddPanel(mPanel);
}

GameScreenId MainScreen::Tick(float dt) {
	mTime += dt;
	const Input& input = mEngine.GetInput();
	if (mStartButton.IsPressed(input)) {
		return ScreenId::play;
	}
	else if (mSettingsButton.IsPressed(input)) {
		return ScreenId::settings;
	}
	else if (mCreditsButton.IsPressed(input)) {
		return ScreenId::credits;
	}
#if ! defined(__ANDROID__) && (defined(_WIN32) || defined(__linux__))
	else if (mQuitButton.IsPressed(input)) {
		mEngine.Quit();
	}
#endif
	return ScreenId::mainMenu;
}

void MainScreen::Draw([[maybe_unused]] GameScreenId topScreen) const {
	if (! mPanel.IsVisible()) {
		return;
	}
	constexpr float dx = 66;
	float           phase = mTime * 4.f;
	float           x = (RefWindowWidth - (NumGemTypes - 1) * dx) * 0.5f;
	BitmapExtParams prm;
	prm.pivot = BitmapPivot::center;
	prm.drawOrder = static_cast<DrawOrderType>(GameDrawOrder::overBackground);
	for (int i = 0; i < NumGemTypes; ++i) {
		const GemDef& gemDef = gemDefs[i];
		prm.orientation = std::sin(phase * .25f + (float)i) * 0.5f;
		mEngine.GetBitmapRenderer().DrawBitmapEx(*sprites[gemDef.sprite], { x, 320.f + std::cos(phase) * 4.f }, prm);
		x += dx;
		phase += 6.28f / static_cast<float>(NumGemTypes);
	}
}

void MainScreen::Enter([[maybe_unused]] GameScreenId prevScreen) {
	mPanel.SetVisible(true);
}

void MainScreen::Exit() {
	mPanel.SetVisible(false);
}
