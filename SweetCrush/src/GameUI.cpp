#include "GameUI.h"
#include "Localization.h"

#include <engine/Color.h>
#include <engine/UI.h>

using namespace Wind;

const float buttonPadding = 48.f;

namespace {

inline Vec2 Spring(Vec2 curr, Vec2 target, float dt) {
	const float stiffness = 0.1f;
	const float damping = 0.8f;
	Vec2        distance = target - curr;
	Vec2        force = distance * stiffness; // Pull toward target
	// Apply friction to the velocity so it settles
	Vec2 velocity = force * damping; //(velocity + force) * damping;
	// Update the actual position
	return curr + velocity * dt;
}

void SquashButton(UITransform& transform, float dt) {
	constexpr float f = 1.025f;
	transform.offset = { 0.f, 4.f };
	transform.scale = { f, 1.f / f };
}

void ReleaseButton(UITransform& transform, float dt) {
	transform.offset = { 0.0, 0.f };
	transform.scale = { 1.f, 1.f };
	// Spring(transform.scale, { 1.f, 1.f }, dt); // LerpEase({ 1.05f, 1.f / 1.05f }, { 1.0f, 1.0f }, std::clamp(t, 0.f, 1.f), EaseOutBounce);;
}

} // namespace

const UITheme uiTheme {
	.textStyle = {
		.color = whiteColor,
		.outlineColor = blackColor,
	},
	.buttonStyle {
		.onIdle = ReleaseButton,
		.onPressed = SquashButton,
		.onHovered = ReleaseButton,
	},
};

constexpr UITextStyle titleTextStyle {
	.color = yellowColor,
	.outlineColor = blackColor,
	.shadowColor = { 0.f, 0.f, 0.f, 100.f },
	.shadow = true,
	.shadowOffset = { 0.f, 4.f },
};

constexpr UITextStyle defaultTextStyle {
	.color = whiteColor,
	.outlineColor = blackColor,
	.shadowColor = { 0.f, 0.f, 0.f, 100.f },
	.shadow = true,
	.shadowOffset = { 0.f, 0.f },
};

constexpr float titleY = 140.f;
constexpr float subTitleY = 280.f;
constexpr float button0_y = 540;
constexpr float button1_y = 660;
constexpr float button2_y = 780;
constexpr float button3_y = 900;
const Color     button0_color = { 255.f, 255.f, 131.f, 255.f };
const Color     button1_color = { 192.f, 222.f, 255.f, 255.f };
const Color     button2_color = { 153.f, 219.f, 175.f, 255.f };
const Color     button3_color = { 255.f, 187.f, 99.f, 255.f };
const float     text0_y = 450;
const float     text1_y = 510;
const Color     panel0_color = { 192.f, 222.f, 255.f, 255.f };
const Color     panel1_color = { 192.f, 222.f, 255.f, 127.f };

UIPanelDesc MakeInfoPanel() {
	return {
		.pos = UIAbsolutePos(0.f, 0.f),
		.size = UIAbsoluteSize(520.f, 400.f),
		.horizontalAlignment = UIHorizAlignment::center,
		.verticalAlignment = UIVertAlignment::center,
		.background = "UI/button.png",
		.backgroundColor = panel0_color,
		._9patch = 16.f,
	};
}

UITextDesc MakeTitle(StringId label, float y) {
	return {
		.stringId = label,
		.pos = { 0.f, y },
		.horizontalAlignment = UIHorizAlignment::center,
		.verticalAlignment = UIVertAlignment::top,
		.font = "title",
		.style = titleTextStyle,
	};
}

UITextDesc MakeScreenText(StringId label, float y) {
	return {
		.stringId = label,
		.pos = { 0.f, y },
		.horizontalAlignment = UIHorizAlignment::center,
		.verticalAlignment = UIVertAlignment::top,
		.font = "smallFont",
		.style = defaultTextStyle,
	};
}

UITextDesc MakeDynScreenText(float y) {
	return {
		.stringId = 0,
		.pos = { 0.f, y },
		.horizontalAlignment = UIHorizAlignment::center,
		.verticalAlignment = UIVertAlignment::top,
		.font = "smallFont",
		.style = defaultTextStyle,
	};
}

Wind::UIButton* MakeMenuButton(UIPanel& container, float y, StringId label, const Color& color, const char* icon) {
	const UIButtonDesc buttonDesc = {
		.pos = UIAbsolutePos(0, y),
		.size = UIAbsoluteSize(520.f, 100.f),
		.horizontalAlignment = UIHorizAlignment::center,
		.verticalAlignment = UIVertAlignment::top,
		.padding = buttonPadding,
		.background = "UI/button.png",
		.backgroundColor = color,
		._9patch = 16.f,
	};
	const UIBitmapDesc iconDesc {
		.fileName = icon,
		.pos = UIZeroPos,
		.size = UIZeroSize,
		.horizontalAlignment = UIHorizAlignment::left,
		.verticalAlignment = UIVertAlignment::center,
		.sizing = UIBitmapSizing::fit,
	};
	const UITextDesc labelDesc {
		.stringId = label,
		.horizontalAlignment = UIHorizAlignment::center,
		.verticalAlignment = UIVertAlignment::center,
		.font = "mediumFont",
		.style = defaultTextStyle,
	};
	UIButton* button = container.Add(buttonDesc);
	button->Add(iconDesc);
	button->Add(labelDesc);
	return button;
}

UIButton* MakeBackButton(Wind::UIPanel& container) {
	return MakeMenuButton(container, button3_y, GameStringId::back, button3_color); //, "icons/back.png");
}

UIButton* MakeCloseButton(Wind::UIPanel& container) {
	const UIButtonDesc buttonDesc = {
		.pos = UIAbsolutePos(-32, 32),
		.size = UIAbsoluteSize(48, 48),
		.horizontalAlignment = UIHorizAlignment::right,
		.verticalAlignment = UIVertAlignment::top,
		.background = "icons/X.png",
		.backgroundColor = whiteColor, //{ 131.f, 255.f, 255.f, 255.f },
	};
	const UIBitmapDesc iconDesc {
		.fileName = "icons/check.png",
		.pos = UIAbsolutePos(0.f, 0.f),
		.size = UIAbsoluteSize(48, 48),
		.horizontalAlignment = UIHorizAlignment::right,
		.verticalAlignment = UIVertAlignment::center,
		.sizing = UIBitmapSizing::user,
		.color = greenColor,
	};
	const UITextDesc labelDesc {
		.stringId = GameStringId::empty,
		.pos = { 0.f, 0.f },
		.horizontalAlignment = UIHorizAlignment::left,
		.verticalAlignment = UIVertAlignment::center,
		.font = "mediumFont",
		.style = defaultTextStyle,
		.visible = false,
	};
	UIButton* button = container.Add(buttonDesc);
	button->Add(iconDesc);
	button->Add(labelDesc);
	return button;
}

UIButton* MakeCheckBox(Wind::UIPanel& container, float y, StringId label, const Color& color) {
	const UIBitmapDesc checkedIconDesc {
		.fileName = "icons/minicheck.png",
		.pos = UIAbsolutePos(0.f, 0.f),
		.horizontalAlignment = UIHorizAlignment::right,
		.verticalAlignment = UIVertAlignment::center,
		.sizing = UIBitmapSizing::fit,
	};
	const UIBitmapDesc uncheckedIconDesc {
		.fileName = "icons/cross.png",
		.pos = UIAbsolutePos(0.f, 0.f),
		.horizontalAlignment = UIHorizAlignment::right,
		.verticalAlignment = UIVertAlignment::center,
		.sizing = UIBitmapSizing::fit,
		.visible = false,
	};
	const UITextDesc labelDesc {
		.stringId = label,
		.pos = { 0.f, 0.f },
		.horizontalAlignment = UIHorizAlignment::left,
		.verticalAlignment = UIVertAlignment::center,
		.font = "mediumFont",
		.style = defaultTextStyle,
	};
	UIButtonDesc buttonDesc = {
		.pos = UIAbsolutePos(0, y),
		.size = UIAbsoluteSize(520.f, 100.f),
		.horizontalAlignment = UIHorizAlignment::center,
		.verticalAlignment = UIVertAlignment::top,
		.padding = buttonPadding,
		.background = "UI/button.png",
		.backgroundColor = color,
		._9patch = 16.f,
	};
	UIButton* checkBox = container.Add(buttonDesc);
	checkBox->Add(labelDesc);
	checkBox->Add(checkedIconDesc);
	checkBox->Add(uncheckedIconDesc);
	return checkBox;
}

UICanvas MakeCanvas() {
	constexpr UICanvasDesc desc {
		.background = "gameartguppy/background.png",
		.padding = 32.f,
	};
	return UICanvas { desc };
}
