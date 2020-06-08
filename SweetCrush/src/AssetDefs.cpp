#include "AssetDefs.h"

using namespace Wind;

const SpriteDef spriteDefs[NumSprites] {
	{ "null.png" },
	{ "gameartguppy/pastry_cookie01.png" },
	{ "gameartguppy/pastry_cookie02.png" },
	{ "gameartguppy/pastry_croissant.png" },
	{ "gameartguppy/pastry_cupcake.png" },
	{ "gameartguppy/pastry_donut.png" },
	{ "gameartguppy/pastry_macaroon.png" },
	{ "gameartguppy/pastry_pie.png" },
	{ "gameartguppy/pastry_poptart01.png" },
	{ "gameartguppy/pastry_poptart02.png" },
	{ "gameartguppy/pastry_starcookie01.png" }, // FIXME icons
	{ "gameartguppy/pastry_starcookie02.png" },
	{ "gameartguppy/pastry_starcookie01.png" },
	{ "gameartguppy/pastry_starcookie02.png" },
	{ "gameartguppy/board.png" },
	{ "sparkle.png" },
};

const BoardTileDef boardTileDefs[NumBoardTiles] {
	{ 14 },
};

const GemDef gemDefs[NumGemTypes] {
	{ 1 }, { 2 }, { 3 }, { 4 }, { 5 }, { 6 }, { 7 }, { 8 }, { 9 },
};

const ObstacleDef obstacleDefs[NumObstacleTypes] {
	{ 0 }, // wall
};

const BoosterDef boosterDefs[NumBoosters] {
	{ 10, 0.5f },
	{ 11, 0.5f },
	{ 12, 0.5f },
	{ 13, 0.5f },
};

Wind::BitmapPtr sprites[NumSprites];
