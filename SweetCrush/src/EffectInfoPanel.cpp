#include "EffectInfoPanel.h"
#include "AssetDefs.h"
#include "GameDrawOrder.h"
#include "Localization.h"
#include "ScreenIds.h"
#include "UIDefs.h"

#include <engine/Engine.h>
#include <engine/Input.h>

using namespace Wind;

namespace {

const UIButtonDesc buttonDesc {
	.pos = UIAbsolutePos(0, 260),
	.size = UIAutoSize,
	.horizontalAlignment = UIHorizAlignment::center,
	.verticalAlignment = UIVertAlignment::top,
};

const UITextDesc textDesc[] {
	{
	    .pos = UIAbsolutePos(0, 60),
	    .size = UIAutoSize,
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::top,
	    .font = "mediumFont",
	    .stringId = GameStringId::empty,
	},
	{
	    .pos = UIAbsolutePos(0, 160),
	    .size = UIAutoSize,
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::top,
	    .font = "smallFont",
	    .stringId = GameStringId::empty,
	},
	{
	    .pos = UIZeroPos,
	    .size = UIAutoSize,
	    .horizontalAlignment = UIHorizAlignment::center,
	    .verticalAlignment = UIVertAlignment::center,
	    .font = "mediumFont",
	    .stringId = GameStringId::ok,
	},
};

const UIBitmapDesc effectIconDesc {
	.fileName = "null.png",
	.pos = UIAbsolutePos(540, 50),
	.size = UIAbsoluteSize(64, 72),
	.horizontalAlignment = UIHorizAlignment::left,
	.verticalAlignment = UIVertAlignment::top,
	.color = whiteColor,
	.blending = UIBlending::on,
};

const UIPanelDesc panelDesc {
	.pos = UIZeroPos,
	.size = UIAbsoluteSize(800, 400),
	.backgroundColor = Color { 0, 0, 0, 200 },
};

} // namespace

EffectInfoScreen::EffectInfoScreen()
    : mTitle(textDesc[0])
    , mText(textDesc[1])
    , mOKButton(buttonDesc, buttonBitmapDesc, textDesc[2])
    , mEffectIcon(effectIconDesc)
    , mPanel(panelDesc)
    , mShowHelp {} {
	for (bool& b : mShowHelp) {
		b = true;
	}

	mPanel.AddBitmap(mEffectIcon);
	mPanel.AddButton(mOKButton);
	mPanel.AddText(mTitle);
	mPanel.AddText(mText);
}

void EffectInfoScreen::LoadAssets(Engine& engine) {
	mPanel.LoadAssets(engine.GetGraphics(), engine.GetTextRenderer());
}

const char* EffectInfoScreen::GetName() const {
	return "EffectInfoPanel";
}

ScreenEvent EffectInfoScreen::Tick(float dt, const Input& input) {
	if (mOKButton.IsClicked(input) ||
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
	mPanel.Draw(uiRenderer, 0); // TODO Rect
}

void EffectInfoScreen::Enter(const ScreenNavArgs& args) {
	// TODO Parse
	EffectType effectType = EffectType::hrocket;
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
	case EffectType::hrocket:
		titleStringId = GameStringId::hRocket;
		textStringId = GameStringId::hRocketDescription;
		break;
	case EffectType::vrocket:
		titleStringId = GameStringId::vRocket;
		textStringId = GameStringId::vRocketDescription;
		break;
	case EffectType::bomb:
		titleStringId = GameStringId::bomb;
		textStringId = GameStringId::bombDescription;
		break;
	case EffectType::miniBomb:
		titleStringId = GameStringId::miniBomb;
		textStringId = GameStringId::miniBombDescription;
		break;
	default:
		break;
	}
	mTitle.SetText(static_cast<StringId>(titleStringId));
	mText.SetText(static_cast<StringId>(textStringId));
	mEffectIcon.SetBitmap(gameTextures[effectIcons[typeIdx]]);
	mPanel.SetVisible(true);
}

void EffectInfoScreen::ParseConfig(const char* varName, const char* varValue) {
}
