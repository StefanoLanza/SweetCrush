#include "UIDefs.h"
#include <engine/Color.h>
#include <engine/UI.h>

using namespace Wind;

const UIBitmapDesc buttonBitmapDesc {
	.fileName = "button.png",
	.pos = UIZeroPos,
	.size = UIAutoSize,
	.color = { 96.f, 194.f, 180.f, 255.f },
};

constexpr TextStyle titleTextStyle {
	yellowColor,
	blackColor,
};

constexpr float titleY = 160.f;
