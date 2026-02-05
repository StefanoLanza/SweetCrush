#include "GameUI.h"
#include <engine/Color.h>
#include <engine/UI.h>

using namespace Wind;

const UIBitmapDesc buttonBitmapDesc {
	.fileName = "button.png",
	.pos = UIZeroPos,
	.size = UIParentSize,
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
	.size = UIAutoSize,
	.horizontalAlignment = UIHorizAlignment::left,
	.verticalAlignment = UIVertAlignment::bottom,
};

const UIBitmapDesc defaultBackButtonBitmapDesc {
	.fileName = "backButton2.png",
	.pos = UIZeroPos,
	.size = UIAutoSize,
	.color = whiteColor,
};

const UISize    defaultButtonSize { 422.f, 100.f, 0.f, 0.f };
constexpr float titleY = 160.f;
constexpr float button0_y = 560;
constexpr float button1_y = 680;
constexpr float button2_y = 800;
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

Wind::UIButton MakeMenuButton(float y, Wind::StringId label) {
	UIButtonDesc buttonDesc = {
		.pos = UIAbsolutePos(0, y),
		.size = defaultButtonSize,
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
		.background = "backButton2.png",
		.backgroundColor = whiteColor, //{ 131.f, 255.f, 255.f, 255.f },
	};
	return Wind::UIButton { buttonDesc }; //, /*iconDesc,*/ labelDesc };
}
