#include "GraphicsSettingsScreen.h"
#include "GameConfig.h"
#include "GameSettings.h"
#include "Localization.h"
#include "ScreenIds.h"
#include "UIDefs.h"
#include <engine/Engine.h>
#include <engine/TextRender.h>
#include <engine/UI.h>

using namespace Wind;

namespace {

const UIButtonDesc filterButtonDesc { { 0, 560, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top };

const UIButtonDesc backButtonDesc { { 0, 920, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::top };

const UITextDesc textDescs[] {
	{ "bigFont",
	  (StringId)GameStringId::graphicsSettings,
	  { 0, titleY, 0, 0 },
	  UIAutoSize,
	  UIHorizAlignment::center,
	  UIVertAlignment::top,
	  titleTextStyle },
	{ "mediumFont", (StringId)GameStringId::nextLanguage, { 0, 0, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::center },
	{ "mediumFont", (StringId)GameStringId::back, { 0, 0, 0, 0 }, UIAutoSize, UIHorizAlignment::center, UIVertAlignment::center },
};

} // namespace

GraphicsSettingsScreen::GraphicsSettingsScreen(Engine& engine, GameSettings& gameSettings)
    : mGameConfig(gameSettings)
    , mTitle(textDescs[0], engine)
    , mFilterButton(MakeButton(filterButtonDesc, buttonBitmapDesc, textDescs[1], engine))
    , mBackButton(MakeButton(backButtonDesc, buttonBitmapDesc, textDescs[2], engine))
    , mPanel(UIDefaultPanelDesc) {
}

const char* GraphicsSettingsScreen::GetName() const {
	return "GraphicsSettingsScreen";
}

void GraphicsSettingsScreen::LoadAssets() {
}

void GraphicsSettingsScreen::BuildUI(UICanvas& canvas) {
	mPanel.AddText(mTitle);
	mPanel.AddButton(mFilterButton);
	mPanel.AddButton(mBackButton);
	canvas.GetPanel().AddPanel(mPanel);
	//	RefreshLanguageButton();
}

GameScreenId GraphicsSettingsScreen::Tick([[maybe_unused]] float dt, const Wind::Input& input) {
	if (mFilterButton.IsPressed(input)) {
	}
	else if (mBackButton.IsPressed(input)) {
		return ScreenId::settings;
	}
	return ScreenId::graphicsSettings;
}

void GraphicsSettingsScreen::Draw([[maybe_unused]] GameScreenId topScreen) const {
}

void GraphicsSettingsScreen::Enter([[maybe_unused]] GameScreenId prevScreen) {
	mPanel.SetVisible(true);
}

void GraphicsSettingsScreen::Exit() {
	mPanel.SetVisible(false);
}

#if 0
void GraphicsSettingsScreen::RefreshLanguageButton() {
	GameStringId stringId = GameStringId::nextLanguage;
	mLanguageButton.GetText()->SetText(static_cast<StringId>(stringId));
}
#endif

void GraphicsSettingsScreen::ParseConfig(const char* varName, const char* varValue) {
}
