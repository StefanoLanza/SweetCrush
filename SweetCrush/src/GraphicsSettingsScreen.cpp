#include "GraphicsSettingsScreen.h"
#include "AppConfig.h"
#include "GameSettings.h"
#include "Localization.h"
#include "ScreenIds.h"
#include "UIDefs.h"

#include <engine/Engine.h>
#include <engine/Input.h>
#include <engine/TextRender.h>
#include <engine/UI.h>

using namespace Wind;

namespace {

const UIButtonDesc filterButtonDesc { { 0, 560, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top };

const UIButtonDesc backButtonDesc { { 0, 920, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top };

const UITextDesc textDescs[] {
	{
	    .pos = { 0, titleY, 0, 0 },
	    .size = UIAutoSize,
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::top,
	    .font = "bigFont",
	    .stringId = (StringId)GameStringId::graphicsSettings,
	    .textStyle = titleTextStyle,
	},
	{

	    .pos = { 0, 0, 0, 0 },
	    .size = UIAutoSize,
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::center,
	    .font = "mediumFont",
	    .stringId = (StringId)GameStringId::nextLanguage,
	},
	{
	    .pos = { 0, 0, 0, 0 },
	    .size = UIAutoSize,
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::center,
	    .font = "mediumFont",
	    .stringId = (StringId)GameStringId::back,
	},
};

constexpr UICanvasDesc canvasDesc {
	.background = "gameartguppy/background.png",
};

} // namespace

GraphicsSettingsScreen::GraphicsSettingsScreen(Engine& engine, GameSettings& gameSettings)
    : mGameConfig(gameSettings)
    , mTitle(textDescs[0], engine.GetTextRenderer())
    , mFilterButton(MakeButton(filterButtonDesc, buttonBitmapDesc, textDescs[1], engine))
    , mBackButton(MakeButton(backButtonDesc, buttonBitmapDesc, textDescs[2], engine))
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
	mCanvas.LoadGraphics(engine.GetGraphics());
}

ScreenTransition GraphicsSettingsScreen::Tick([[maybe_unused]] float dt, const Wind::Input& input) {
	if (mFilterButton.IsPressed(input)) {
		// TODO
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

void GraphicsSettingsScreen::Draw(Wind::UIRenderer& uiRenderer) {
	mCanvas.Draw(RefWindowWidth, RefWindowHeight, uiRenderer, 0);
}

void GraphicsSettingsScreen::Enter([[maybe_unused]] ScreenId prevScreen, const void* payload) {
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
