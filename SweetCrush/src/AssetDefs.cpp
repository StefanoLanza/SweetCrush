#include "AssetDefs.h"

using namespace Wind;

const SpriteDef spriteDefs[NumSprites] {
	{ "null.png" },
	{ "pastries/pastry_cookie01.png" },
	{ "pastries/pastry_cookie02.png" },
	{ "pastries/pastry_croissant.png" },
	{ "pastries/pastry_cupcake.png" },
	{ "pastries/pastry_donut.png" },
	{ "pastries/pastry_macaroon.png" },
	{ "pastries/pastry_pie.png" },
	{ "pastries/pastry_poptart01.png" },
	{ "pastries/pastry_poptart02.png" },
	{ "bomb_red.png" },
	{ "bomb_blue.png" },
	{ "pastries/pastry_starcookie01.png" },
	{ "pastries/pastry_starcookie02.png" },
	{ "board.png" },
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
	{ 10, BoosterType::miniBomb, 0.f },
	{ 11, BoosterType::bomb, 0.f },
	{ 12, BoosterType::yellowStar, 0.5f },
	{ 13, BoosterType::redStar, 0.5f },
};

Wind::BitmapPtr sprites[NumSprites];
