#pragma once

#include <cstdint>

enum GameDrawOrder : uint32_t {
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
