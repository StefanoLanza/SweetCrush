#include "GraphicsSettingsScreen.h"
#include "AppConfig.h"
#include "GameSettings.h"
#include "Localization.h"
#include "ScreenIds.h"
#include "GameUI.h"

#include <engine/Engine.h>
#include <engine/Input.h>
#include <engine/TextRender.h>
#include <engine/UI.h>

using namespace Wind;

namespace {

const UIButtonDesc languageButtonDesc { { 0, 560, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top };

const UIButtonDesc backButtonDesc { { 0, 920, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top };

const UITextDesc textDescs[] {
	{
	    .pos = { 0, titleY },
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::top,
	    .font = "screenTitle",
	    .stringId = GameStringId::graphicsSettings,
	},
	{
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::center,
	    .font = "mediumFont",
	    .stringId = GameStringId::nextLanguage,
	},
	{
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::center,
	    .font = "mediumFont",
	    .stringId = GameStringId::back,
	},
};

constexpr UICanvasDesc canvasDesc {
	.background = "gameartguppy/background.png",
};

} // namespace

GraphicsSettingsScreen::GraphicsSettingsScreen(GameSettings& gameSettings)
    : mGameConfig(gameSettings)
    , mTitle(textDescs[0], titleTextStyle)
    , mFilterButton(languageButtonDesc, buttonBitmapDesc, textDescs[1])
    , mBackButton(defaultBackButtonDesc, defaultBackButtonBitmapDesc)
    , mCanvas(canvasDesc) {
	// Setup UI
	mCanvas.AddText(mTitle);
	mCanvas.AddButton(mFilterButton);
	mCanvas.AddButton(mBackButton);
}

const char* GraphicsSettingsScreen::GetName() const {
	return "GraphicsSettingsScreen";
}

void GraphicsSettingsScreen::LoadAssets(Engine& engine) {
	mCanvas.LoadAssets(engine.GetGraphics(), engine.GetFontManager());
}

ScreenEvent GraphicsSettingsScreen::Tick([[maybe_unused]] float dt, const Wind::Input& input) {
	if (mFilterButton.IsClicked(input)) {
		// TODO
	}

#if defined(__ANDROID__) || defined(__OHOS__)
	if (input.GetKeyJustPressed(SDLK_AC_BACK) ||
#elif defined(_WIN32) || defined(__linux__)
	if (input.GetKeyJustPressed(SDLK_ESCAPE) ||
#endif
	    mBackButton.IsClicked(input)) {
		return GoBack(ScreenTransition::slideRight);
	}
	return Continue();
}

void GraphicsSettingsScreen::Draw(Wind::UIRenderer& uiRenderer, float dt) {
	mCanvas.Draw(RefWindowWidth, RefWindowHeight, uiRenderer, 0);
}

void GraphicsSettingsScreen::Enter(const ScreenNavArgs& args) {
}

void GraphicsSettingsScreen::Exit() {
}

#if 0
void GraphicsSettingsScreen::RefreshLanguageButton() {
	GameStringId stringId = GameStringId::nextLanguage;
	mLanguageButton.GetText()->SetText(static_cast<StringId>(stringId));
}
#endif

void GraphicsSettingsScreen::ParseConfig(const char* varName, const char* varValue) {
}
