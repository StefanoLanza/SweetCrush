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
	{ "gameartguppy/pastry_starcookie01.png" },
	{ "boosters/bomb_green.png" }, // TODO Replace
	{ "boosters/bomb_blue.png" },
	{ "boosters/bomb_red.png" },
	{ "gameartguppy/board.png" },
	{ "sparkle.png" },
	{ "gameartguppy/block_ice_01a.png" },
	{ "gameartguppy/block_ice_02.png" },
	{ "gameartguppy/block_ice_03.png" },
	{ "outline.png" },
	{ "glow.png" },
	{ "hrzStripes.png" },
	{ "vrtStripes.png" },
};

const BoardTileDef boardTileDefs[NumBoardTiles] {
	{ 16 },
};

const PieceDef pieceDefs[NumPieceTypes] {
	{ 1 }, { 2 }, { 3 }, { 4 }, { 5 }, { 6 }, { 7 }, { 8 }, { 9 }, { 10 }, { 11 },
};

const int pieceHrzStripesBitmaps {};

const ObstacleDef obstacleDefs[NumObstacleTypes] {
	{ 0 }, // wall
};

const BoosterDef boosterDefs[NumBoosters] {
	{ 12 },
	{ 13 },
	{ 14 },
	{ 15 },
};

Wind::TexturePtr sprites[NumSprites];

const int sparkleSprite = 17;
const int iceSprite = 18;
const int brokenIceSprite = 19;
const int selectionSprite = 21;
const int glowSprite = 22;
const int starSprite = 12;
const int hrzStripesSprite = 23;
const int vrtStripesSprite = 24;