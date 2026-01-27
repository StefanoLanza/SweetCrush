#pragma once

#include <engine/DrawOrder.h>

enum GameDrawOrder : Wind::DrawOrderType {
	background = 0,
	backgroundTile,
	boardPiece,
	ice,
	overlays,
	UI,
	// 32 slots for UI widgets
	overUI = UI + 32,
	mousePointer = 200,
};
