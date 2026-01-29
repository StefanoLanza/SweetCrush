#include "UIDefs.h"
#include <engine/Color.h>
#include <engine/UI.h>

using namespace Wind;

const UIBitmapDesc buttonBitmapDesc {
	.fileName = "button.png",
	.pos = UIZeroPos,
	.size = UIAutoSize,
	.color = { 131.f, 255.f, 255.f, 255.f },
};

constexpr TextStyle titleTextStyle {
	yellowColor,
	blackColor,
};

const UIButtonDesc defaultBackButtonDesc {
	.pos = UIAbsolutePos(32, -32),
	.size = UIAutoSize,
	.horizontalAlignment = UIHorizAlignment::left,
	.verticalAlignment = UIVertAlignment::bottom,
};

const UIBitmapDesc defaultBackButtonBitmapDesc {
	.fileName = "backButton.png",
	.pos = UIZeroPos,
	.size = UIAutoSize,
	.color = yellowColor,
};

constexpr float titleY = 160.f;
constexpr float button0_y = 560;
constexpr float button1_y = 680;
constexpr float button2_y = 800;
const float     text0_y = 450;
const float     text1_y = 510;
