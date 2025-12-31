#include "UIDefs.h"
#include <engine/Color.h>
#include <engine/UI.h>

using namespace Wind;

const UIBitmapDesc buttonBitmapDesc {
	"button.png",
	UIZeroPos,
	UIAutoSize,
};

const UIBitmapDesc shortButtonBitmapDesc {
	"shortbutton.png",
	UIZeroPos,
	UIAutoSize,
};

constexpr TextStyle titleTextStyle {
	yellowColor,
	blackColor,
};

constexpr float titleY = 160.f;
