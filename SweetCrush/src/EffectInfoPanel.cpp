#include "EffectInfoPanel.h"
#include "AssetDefs.h"
#include "GameDrawOrder.h"
#include "GameUI.h"
#include "Localization.h"
#include "ScreenIds.h"

#include <engine/Engine.h>
#include <engine/Input.h>

using namespace Wind;

namespace {

const UITextDesc textDesc[] {
	{
	    .stringId = GameStringId::empty,
	    .pos = { 0.f, 160.f },
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::top,
	    .font = "smallFont",
        .style = defaultTextStyle,
	},
};

const UIBitmapDesc effectIconDesc {
	.fileName = "null.png",
	.pos = UIAbsolutePos(540, 50),
	.size = UIAbsoluteSize(64, 72),
	.horizontalAlignment = UIHorizAlignment::left,
	.verticalAlignment = UIVertAlignment::top,
};

const UIPanelDesc panelDesc {
	.pos = UIZeroPos,
	.size = UIAbsoluteSize(800, 400),
	.backgroundColor = Color { 0, 0, 0, 200 },
};

} // namespace

EffectInfoScreen::EffectInfoScreen()
    : mShowHelp {} {
	for (bool& b : mShowHelp) {
		b = true;
	}

#if 0
	UIPanel*   panel;
	//panelDesc
	mEffectIcon = panel->Add(effectIconDesc);
	mOKButton = panel->Add(MakeMenuButton(50, GameStringId::ok));
	mTitle = panel->Add(MakeTitle(GameStringId::empty));
	panel->Add(textDesc[0]);
#endif
}

void EffectInfoScreen::LoadAssets(Engine& engine) {
	//panel->LoadAssets(engine.GetGraphics(), engine.GetFontManager());
}

const char* EffectInfoScreen::GetName() const {
	return "EffectInfoPanel";
}

ScreenEvent EffectInfoScreen::Tick(float dt, const Input& input) {
	//mPanel->Tick(dt);
	//mPanel->HandleInput(input);
	if (mOKButton->IsClicked() ||
#if defined(_WIN32) || defined(__linux__)
	    input.GetKeyJustPressed(SDLK_ESCAPE)) {
#else
			0
}
#endif
		return GoBack();
	}
	return Continue();
}

void EffectInfoScreen::Draw(Wind::UIRenderer& uiRenderer, float dt) {
	//FIXME mPanel.Write(uiRenderer, 0); // TODO Rect
}

void EffectInfoScreen::Enter(const ScreenNavArgs& args) {
	// TODO Parse
	EffectType effectType = EffectType::hStriped;
	ShowHelp(effectType);
}

void EffectInfoScreen::Exit() {
}

void EffectInfoScreen::ShowHelp(EffectType effectType) {
	const int typeIdx = static_cast<int>(effectType);
	if (! mShowHelp[typeIdx]) {
		return;
	}
	if (mShowHelp[typeIdx]) {
		mShowHelp[typeIdx] = false;
	}
	GameStringId titleStringId = GameStringId::empty;
	GameStringId textStringId = GameStringId::empty;
	switch (effectType) {
	case EffectType::hStriped:
		titleStringId = GameStringId::hStriped;
		textStringId = GameStringId::hStripedDescription;
		break;
	case EffectType::vStriped:
		titleStringId = GameStringId::vStriped;
		textStringId = GameStringId::vStripedDescription;
		break;
	case EffectType::wrapped:
		titleStringId = GameStringId::wrapped;
		textStringId = GameStringId::wrappedDescription;
		break;
	case EffectType::colorBomb:
		titleStringId = GameStringId::colorBomb;
		textStringId = GameStringId::colorBombDescription;
		break;
	default:
		break;
	}
	mTitle->SetText(static_cast<StringId>(titleStringId));
	mText->SetText(static_cast<StringId>(textStringId));
	// FIXME mEffectIcon->SetBitmap(gameTextures[effectIcons[typeIdx]]);
	//mPanel->SetVisible(true);
}

void EffectInfoScreen::ParseConfig(const char* varName, const char* varValue) {
}
