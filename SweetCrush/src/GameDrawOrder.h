#pragma once

#include <engine/DrawOrder.h>

enum class GameDrawOrder : Wind::DrawOrderType {
	overBackground = Wind::DrawOrder::background + 1,
	backgroundTile = Wind::DrawOrder::user,
	boardPiece,
	ice,
	overlays,
	textOverUI = Wind::DrawOrder::UI + 50,
};
