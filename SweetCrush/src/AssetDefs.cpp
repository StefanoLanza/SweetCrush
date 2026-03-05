#include "AssetDefs.h"

using namespace Wind;

Wind::TexturePtr gameTextures[NumGameTextures];

// TODO Atlas row cols and tile size
const char* gameTexturePath[NumGameTextures] {
	"gameartguppy/pastry_cookie01.png",
	"gameartguppy/pastry_croissant.png",
	"gameartguppy/pastry_cookie02.png",
	"gameartguppy/pastry_cupcake.png",
	"gameartguppy/pastry_donut.png",
	"gameartguppy/pastry_macaroon.png",
	"gameartguppy/pastry_poptart01.png",
	"gameartguppy/pastry_pie.png",
	"gameartguppy/pastry_poptart02.png",
	"gameartguppy/board.png",
	// Effects
	"gameartguppy/pastry_starcookie01.png",
	"gameartguppy/pastry_starcookie01.png",
	"gameartguppy/pastry_starcookie02.png",     // wrapped
	"gameartguppy/candy_06_jawbreaker_100.png", // color bomb
	// Ice
	"gameartguppy/block_ice_01a.png",
	"gameartguppy/block_ice_02.png",
	"gameartguppy/block_ice_03.png",
	"outline.png",
	"glow.png",
	// Boosters
	"gameartguppy/candy_05_candycane_100.png",
	"gameartguppy/candy_04_lollipop_100.png",
	"gameartguppy/candy_09_gummibear_100.png",
	"blast.png",
};

const SpriteID pieceIcons[NumPieceTypes] { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
const SpriteID boardTileIcons[NumBoardTiles] { 9 };
const SpriteID obstacleIcons[NumObstacleTypes] { 0 };
const SpriteID colorBombIcon = 13;
const SpriteID iceSprites[3] = { 14, 15, 16 };

const SpriteID boosterIcons[NumBoosters] = {
	19,
	20,
	21,
};

const SpriteID selectionSprite = 17;
const SpriteID glowSprite = 18;
const SpriteID starSprite = 0; // FIXME
const SpriteID blastSprite = 22;
