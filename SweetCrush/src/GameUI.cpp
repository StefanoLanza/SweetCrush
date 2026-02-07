#include "GameUI.h"
#include <engine/Color.h>
#include <engine/UI.h>

using namespace Wind;

const UIBitmapDesc buttonBitmapDesc {
	.fileName = "button.png",
	.pos = UIZeroPos,
	.sizing = UIBitmapSizing::stretch,
	.color = { 131.f, 255.f, 255.f, 255.f },
};

constexpr TextStyle titleTextStyle {
	yellowColor,
	blackColor,
};

constexpr TextStyle defaultTextStyle {
	whiteColor,
	blackColor,
};

const UIButtonDesc defaultBackButtonDesc {
	.pos = UIAbsolutePos(32, -32),
	.size = UIAbsoluteSize(64, 64),
	.horizontalAlignment = UIHorizAlignment::left,
	.verticalAlignment = UIVertAlignment::bottom,
};

const UIBitmapDesc defaultBackButtonBitmapDesc {
	.fileName = "backButton2.png",
	.pos = UIZeroPos,
	.sizing = UIBitmapSizing::fit,
	.color = whiteColor,
};

constexpr float titleY = 160.f;
constexpr float subTitleY = 280.f;
constexpr float button0_y = 560;
constexpr float button1_y = 680;
constexpr float button2_y = 800;
constexpr float button3_y = 440;
const float     text0_y = 450;
const float     text1_y = 510;

Wind::UIText MakeTitleText(Wind::StringId label) {
	const UITextDesc desc {
		.pos = { 0.f, titleY },
		.horizontalAlignment = UIHorizAlignment::center,
		.verticalAlignment = UIVertAlignment::top,
		.font = "screenTitle", //"bigFont",
		.stringId = label,
	};
	return Wind::UIText { desc, titleTextStyle };
}

Wind::UIText MakeSubTitleText(Wind::StringId label) {
	const UITextDesc desc {
		.pos = { 0.f, subTitleY },
		.horizontalAlignment = UIHorizAlignment::center,
		.verticalAlignment = UIVertAlignment::top,
		.font = "screenTitle",
		.stringId = label,
	};
	return Wind::UIText { desc, titleTextStyle };
}

Wind::UIText MakeScreenText(Wind::StringId label, float y) {
	const UITextDesc desc {
		.pos = { 0.f, y },
		.horizontalAlignment = UIHorizAlignment::center,
		.verticalAlignment = UIVertAlignment::top,
		.font = "smallFont",
		.stringId = label,
	};
	return Wind::UIText { desc, defaultTextStyle };
}

Wind::UIText MakeDynScreenText(float y) {
	const UITextDesc desc {
		.pos = { 0.f, y },
		.horizontalAlignment = UIHorizAlignment::center,
		.verticalAlignment = UIVertAlignment::top,
		.font = "smallFont",
		.stringId = 0,
	};
	return Wind::UIText { desc, defaultTextStyle };
}

Wind::UIButton MakeMenuButton(float y, Wind::StringId label) {
	UIButtonDesc buttonDesc = {
		.pos = UIAbsolutePos(0, y),
		.size = UIAbsoluteSize(420.f, 100.f),
		.horizontalAlignment = UIHorizAlignment::center,
		.verticalAlignment = UIVertAlignment::top,
		.background = "button.png",
		.backgroundColor = { 131.f, 255.f, 255.f, 255.f },
		._9patch = { 16.f, 0.f, 0.f, 0.f },
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
	};
	return Wind::UIButton { buttonDesc, /*iconDesc,*/ labelDesc };
}

Wind::UIButton MakeQuitButton() {
	UIButtonDesc buttonDesc = {
		.pos = UIAbsolutePos(32, -32),
		.size = UIAbsoluteSize(64, 64),
		.horizontalAlignment = UIHorizAlignment::left,
		.verticalAlignment = UIVertAlignment::bottom,
		.background = "quitButton.png",
		.backgroundColor = whiteColor, //{ 131.f, 255.f, 255.f, 255.f },
	};
	return Wind::UIButton { buttonDesc }; //, /*iconDesc,*/ labelDesc };
}

Wind::UIButton MakeBackButton() {
	UIButtonDesc buttonDesc = {
		.pos = UIAbsolutePos(32, -32),
		.size = UIAbsoluteSize(64, 64),
		.horizontalAlignment = UIHorizAlignment::left,
		.verticalAlignment = UIVertAlignment::bottom,
		.background = "back.png",
		.backgroundColor = whiteColor, //{ 131.f, 255.f, 255.f, 255.f },
	};
	return Wind::UIButton { buttonDesc }; //, /*iconDesc,*/ labelDesc };
}

Wind::UIButton MakeToggleButton(float y, Wind::StringId label) {
	UIButtonDesc buttonDesc = {
		.pos = UIAbsolutePos(0, y),
		.size = UIAbsoluteSize(420.f, 100.f),
		.horizontalAlignment = UIHorizAlignment::center,
		.verticalAlignment = UIVertAlignment::top,
		.background = "button.png",
		.backgroundColor = { 131.f, 255.f, 255.f, 255.f },
		._9patch = { 16.f, 0.f, 0.f, 0.f },
		.toggleMode = true,
		.toggled = true,
	};
	const float        padding = 48.f;
	const UIBitmapDesc iconDesc {
		.fileName = "checkmark.png",
		.pos = UIAbsolutePos(-padding, 0.f),
		.horizontalAlignment = UIHorizAlignment::right,
		.verticalAlignment = UIVertAlignment::center,
		.sizing = UIBitmapSizing::fit,
		.color = greenColor,
	};
	const UITextDesc labelDesc {
		.pos = { padding, 0.f },
		.horizontalAlignment = UIHorizAlignment::left,
		.verticalAlignment = UIVertAlignment::center,
		.font = "mediumFont",
		.stringId = label,
	};
	return Wind::UIButton { buttonDesc, iconDesc, labelDesc };
}

Wind::UICanvas MakeCanvas() {
	constexpr UICanvasDesc desc {
		.background = "gameartguppy/background.png",
	};
	return UICanvas { desc };
}
