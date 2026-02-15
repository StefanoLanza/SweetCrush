#include "GameUI.h"
#include "Localization.h"

#include <engine/Color.h>
#include <engine/UI.h>

using namespace Wind;

const Wind::UIBitmapDesc noIconDesc {};
const float              buttonPadding = 48.f;

const UITextDesc noLabelDesc {
	.stringId = GameStringId::empty,
	.visible = false,
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
	.shadowOffset = { 0.f, 4.f },
};

constexpr float titleY = 160.f;
constexpr float subTitleY = 280.f;
constexpr float button0_y = 560;
constexpr float button1_y = 680;
constexpr float button2_y = 800;
constexpr float button3_y = 920;
const Color     button0_color = { 255.f, 255.f, 131.f, 255.f };
const Color     button1_color = { 192.f, 222.f, 255.f, 255.f };
const Color     button2_color = { 153.f, 219.f, 175.f, 255.f };
const Color     button3_color = { 255.f, 187.f, 99.f, 255.f };
const float     text0_y = 450;
const float     text1_y = 510;
const Color     panel0_color = { 192.f, 222.f, 255.f, 255.f };

Wind::UIPanel MakeInfoPanel() {
	const UIPanelDesc desc {
		.pos = UIAbsolutePos(0.f, 0.f),
		.size = UIAbsoluteSize(520.f, 500.f),
		.horizontalAlignment = UIHorizAlignment::center,
		.verticalAlignment = UIVertAlignment::center,
		.background = "UI/button.png",
		.backgroundColor = panel0_color,
		._9patch = 16.f,
	};
	return UIPanel { desc };
}

Wind::UIText MakeTitleText(Wind::StringId label) {
	const UITextDesc desc {
		.stringId = label,
		.pos = { 0.f, titleY },
		.horizontalAlignment = UIHorizAlignment::center,
		.verticalAlignment = UIVertAlignment::top,
		.font = "screenTitle", //"bigFont",
		.style = titleTextStyle,
	};
	return Wind::UIText { desc };
}

Wind::UIText MakeSubTitleText(Wind::StringId label) {
	const UITextDesc desc {
		.stringId = label,
		.pos = { 0.f, subTitleY },
		.horizontalAlignment = UIHorizAlignment::center,
		.verticalAlignment = UIVertAlignment::top,
		.font = "screenTitle",
		.style = titleTextStyle,
	};
	return Wind::UIText { desc };
}

Wind::UIText MakeScreenText(Wind::StringId label, float y) {
	const UITextDesc desc {
		.stringId = label,
		.pos = { 0.f, y },
		.horizontalAlignment = UIHorizAlignment::center,
		.verticalAlignment = UIVertAlignment::top,
		.font = "smallFont",
		.style = defaultTextStyle,
	};
	return Wind::UIText { desc };
}

Wind::UIText MakeDynScreenText(float y) {
	const UITextDesc desc {
		.stringId = 0,
		.pos = { 0.f, y },
		.horizontalAlignment = UIHorizAlignment::center,
		.verticalAlignment = UIVertAlignment::top,
		.font = "smallFont",
		.style = defaultTextStyle,
	};
	return Wind::UIText { desc };
}

Wind::UIButton MakeMenuButton(float y, Wind::StringId label, const Color& color, const char* icon) {
#if 0
	UIButtonDesc buttonDesc = {
		.pos = UIAbsolutePos(0, y),
		.size = UIAbsoluteSize(520.f, 100.f),
		.horizontalAlignment = UIHorizAlignment::center,
		.verticalAlignment = UIVertAlignment::top,
		.background = "UI/buttonOrange.png",
		.backgroundColor = whiteColor, //{ 131.f, 255.f, 255.f, 255.f },
		._9patch = 0.f,                // 16.f,
	};
	/*	const UIBitmapDesc iconDesc {
	        .fileName = "button.png",
	        .pos = UIZeroPos,
	        .size = UIParentSize,
	        .color = { 131.f, 255.f, 255.f, 255.f },
	    };*/
	const UITextDesc labelDesc {
		.horizontalAlignment = UIHorizAlignment::center,
		.verticalAlignment = UIVertAlignment::center,
		.font = "mediumFont",
		.stringId = label,
		.style = {
	.color = whiteColor,
	.outlineColor = whiteColor,
	.shadowColor = { 0.f, 0.f, 0.f, 100.f },
	.shadow = true,
	.shadowOffset = { 0.f, 8.f },
},
#else
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
#endif
	};
	return Wind::UIButton { buttonDesc, iconDesc, labelDesc };
}

Wind::UIButton MakeBackButton() {
	return MakeMenuButton(button3_y, GameStringId::back, button3_color);
#if 0
	UIButtonDesc buttonDesc = {
		.pos = UIAbsolutePos(32, -32),
		.size = UIAbsoluteSize(96, 96),
		.horizontalAlignment = UIHorizAlignment::left,
		.verticalAlignment = UIVertAlignment::bottom,
		.background = "icons/backButton.png",
		.backgroundColor = whiteColor, //{ 131.f, 255.f, 255.f, 255.f },
	};
	return Wind::UIButton { buttonDesc }; //, /*iconDesc,*/ labelDesc };
#endif
}

Wind::UIButton MakeCloseButton() {
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
		.visible = false,
	};
	return Wind::UIButton { buttonDesc, iconDesc, labelDesc };
}

Wind::UICheckBox MakeCheckBox(float y, Wind::StringId label, const Color& color) {
	const UIBitmapDesc checkedIconDesc {
		.fileName = "icons/minicheck.png",
		.pos = UIAbsolutePos(0.f, 0.f),
		.size = UIAbsoluteSize(32, 32),
		.horizontalAlignment = UIHorizAlignment::right,
		.verticalAlignment = UIVertAlignment::center,
		.sizing = UIBitmapSizing::fit,
	};
	const UIBitmapDesc uncheckedIconDesc {
		.fileName = "icons/cross.png",
		.pos = UIAbsolutePos(0.f, 0.f),
		.size = UIAbsoluteSize(96, 96),
		.horizontalAlignment = UIHorizAlignment::right,
		.verticalAlignment = UIVertAlignment::center,
		.sizing = UIBitmapSizing::fit,
	};
	const UITextDesc labelDesc {
		.stringId = label,
		.pos = { 0.f, 0.f },
		.horizontalAlignment = UIHorizAlignment::left,
		.verticalAlignment = UIVertAlignment::center,
		.font = "mediumFont",
	};
	UICheckBoxDesc buttonDesc = {
		.pos = UIAbsolutePos(0, y),
		.size = UIAbsoluteSize(520.f, 100.f),
		.horizontalAlignment = UIHorizAlignment::center,
		.verticalAlignment = UIVertAlignment::top,
		.padding = buttonPadding,
		.background = "UI/button.png",
		.backgroundColor = color,
		._9patch = 16.f,
		.toggled = true,
		.label = labelDesc,
		.checkedIcon = checkedIconDesc,
		.uncheckedIcon = uncheckedIconDesc,
	};
	return Wind::UICheckBox { buttonDesc, {} };
}

Wind::UICanvas MakeCanvas() {
	constexpr UICanvasDesc desc {
		.background = "gameartguppy/background.png",
	};
	return UICanvas { desc };
}
