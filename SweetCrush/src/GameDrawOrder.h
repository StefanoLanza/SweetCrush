#pragma once

#include <engine/DrawOrder.h>

enum GameDrawOrder : Wind::DrawOrderType {
	UI = 0,
	overUI = UI + 32,
	backgroundTile,
	boardPiece,
	ice,
	overlays,
};
