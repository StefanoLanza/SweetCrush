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
	{ "gameartguppy/pastry_starcookie01.png" },
	{ "gameartguppy/pastry_starcookie02.png" },
	{ "boosters/bomb_yellow.png" },
	{ "boosters/bomb_green.png" },
	{ "boosters/bomb_blue.png" },
	{ "boosters/bomb_red.png" },
	{ "gameartguppy/board.png" },
	{ "sparkle.png" },
};

const BoardTileDef boardTileDefs[NumBoardTiles] {
	{ 16 },
};

const GemDef gemDefs[NumGemTypes] {
	{ 1 }, { 2 }, { 3 }, { 4 }, { 5 }, { 6 }, { 7 }, { 8 }, { 9 }, { 10 }, { 11 },
};

const ObstacleDef obstacleDefs[NumObstacleTypes] {
	{ 0 }, // wall
};

const BoosterDef boosterDefs[NumBoosters] {
	{ 12, 0.1f },
	{ 13, 0.1f },
	{ 14, 0.1f },
	{ 15, 0.1f },
};

Wind::BitmapPtr sprites[NumSprites];

const int sparkleSprite = 17;
