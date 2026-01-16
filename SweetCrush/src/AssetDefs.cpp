#include "AssetDefs.h"

using namespace Wind;

Wind::TexturePtr pastryAtlas;
Wind::TexturePtr sprites[NumSprites];

const SpriteDef spriteDefs[NumSprites] {
	{ "null.png" },
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

const int boardTileDefs[NumBoardTiles] { 4 };

const int pieceIcons[NumPieceTypes] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

const int obstacleIcons[NumObstacleTypes] { 0 };

const int effectIcons[NumEffects] {
	0,
	1,
	2,
	3,
};

const int sparkleSprite = 5;
const int iceSprite = 6;
const int brokenIceSprite = 7;
const int selectionSprite = 9;
const int glowSprite = 10;
const int starSprite = 0; // FIXME
const int hrzStripesSprite = 11;
const int vrtStripesSprite = 12;