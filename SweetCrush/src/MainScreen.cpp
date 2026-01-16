#include "MainScreen.h"
#include "AssetDefs.h"
#include "Constants.h"
#include "GameDrawOrder.h"
#include "Localization.h"
#include "ScreenIds.h"
#include "UIDefs.h"

#include <engine/BitmapRender.h>
#include <engine/Engine.h>
#include <engine/TextRender.h>
#include <engine/UI.h>

#include <cmath>

using namespace Wind;

namespace {

#if defined(__ANDROID__) || defined(__OHOS__)
// No quit button on mobiles
const UIButtonDesc buttonDescs[] {
	{ UIAbsolutePos(0, 680), UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top },
	{ UIAbsolutePos(0, 800), UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top },
	{ UIAbsolutePos(0, 920), UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top },
};
#else
const UIButtonDesc buttonDescs[] {
	{ UIAbsolutePos(0, 560), UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top },
	{ UIAbsolutePos(0, 680), UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top },
	{ UIAbsolutePos(0, 800), UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top },
	{ UIAbsolutePos(0, 920), UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top },
};
#endif
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
#if ! defined(__ANDROID__)
    , mQuitButton(MakeButton(buttonDescs[3], buttonBitmapDesc, textDescs[4], engine))
#endif
    , mPanel(UIDefaultPanelDesc)
    , mTime(0) {
}

const char* MainScreen::GetName() const {
	return "MainScreen";
}

void MainScreen::LoadAssets() {
}

void MainScreen::BuildUI(UICanvas& canvas) {
	mPanel.AddText(mTitle);
	mPanel.AddButton(mStartButton);
	mPanel.AddButton(mSettingsButton);
	mPanel.AddButton(mCreditsButton);
#if ! defined(__ANDROID__)
	mPanel.AddButton(mQuitButton);
#endif
	canvas.GetPanel().AddPanel(mPanel);
}

GameScreenId MainScreen::Tick(float dt, const Wind::Input& input) {
	mTime += dt;
	if (mStartButton.IsPressed(input)) {
		return ScreenId::play;
	}
	else if (mSettingsButton.IsPressed(input)) {
		return ScreenId::settings;
	}
	else if (mCreditsButton.IsPressed(input)) {
		return ScreenId::credits;
	}
#if ! defined(__ANDROID__) && ! defined(__OHOS__)
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
	constexpr float dx = TileWidth + 2;
	float           phase = mTime * 4.f;
	float           x = (RefWindowWidth - (NumPieceTypes - 4) * dx) * 0.5f;
	BitmapExtParams prm;
	prm.pivot = BitmapPivot::center;
	prm.drawOrder = static_cast<DrawOrderType>(GameDrawOrder::overBackground);
	prm.blending = true;
	for (int i = 0; i < NumPieceTypes - 3; ++i) {
		prm.orientation = std::sin(phase * .25f + (float)i) * 0.5f;
		prm.width = TileWidth;
		prm.height = TileHeight;
		int id = pieceIcons[i];
		prm.texRect = { (float)(id % 4), (float)(id / 4), TileWidth / 256.0f, TileHeight / 256.0f };
		mEngine.GetBitmapRenderer().DrawBitmapEx(*pastryAtlas, { x, 380.f + std::cos(phase) * 4.f }, prm);
		x += dx;
		phase += 6.28f / static_cast<float>(NumPieceTypes);
	}
}

void MainScreen::Enter([[maybe_unused]] GameScreenId prevScreen) {
	mPanel.SetVisible(true);
}

void MainScreen::Exit() {
	mPanel.SetVisible(false);
}

void MainScreen::ParseConfig(const char* varName, const char* varValue) {
}
