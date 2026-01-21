#pragma once

#include <engine/DrawOrder.h>

enum GameDrawOrder : Wind::DrawOrderType {
	backgroundTile = Wind::DrawOrder::background + 1,
	boardPiece,
	ice,
	overlays,
	UI,
	// 32 slots for UI widgets
	overUI = UI + 32,
};
