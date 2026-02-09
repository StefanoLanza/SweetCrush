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
	"boosters/bomb_green.png", // TODO Replace
	"boosters/bomb_blue.png",
	"boosters/bomb_red.png",
	"gameartguppy/block_ice_01a.png",
	"gameartguppy/block_ice_02.png",
	"gameartguppy/block_ice_03.png",
	"outline.png",
	"glow.png",
	"hrzStripes.png",
	"vrtStripes.png",
	// Boosters
	"gameartguppy/candy_05_candycane_100.png",
	"gameartguppy/candy_04_lollipop_100.png",
	"gameartguppy/candy_09_gummibear_100.png",
	"blast.png",
	// Pets
	"gameartguppy/pet_bird_160x160.png",
	"gameartguppy/pet_turtle_160x160.png",
	"gameartguppy/pet_fish_160x160.png",
};

const SpriteID pieceIcons[NumPieceTypes] { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
const SpriteID boardTileIcons[NumBoardTiles] { 9 };
const SpriteID obstacleIcons[NumObstacleTypes] { 0 };
const SpriteID effectIcons[NumEffects] {
	10,
	11,
	12,
	13,
};

const SpriteID boosterIcons[NumBoosters] = {
	20,
	21,
	22,
};

const SpriteID iceSprites[3] = { 13, 14, 15 };
const SpriteID selectionSprite = 16;
const int      glowSprite = 17;
const int      starSprite = 0; // FIXME
const int      blastSprite = 23;

const int petSprites[] = { 24, 25, 26 };
