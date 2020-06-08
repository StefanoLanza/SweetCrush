#pragma once

#include "Boosters.h"
#include <engine/FwdDecl.h>

constexpr int NumSprites = 16;
constexpr int NumGemTypes = 9;
constexpr int NumBoardTiles = 1;
constexpr int NumObstacleTypes = 1;

struct SpriteDef {
	/*	const char** bitmaps;
	    int          numBitmaps;
	    float        animationFreq;*/
	const char* bitmap;
};

struct GemDef {
	int sprite;
};

struct ObstacleDef {
	int sprite;
};

struct BoosterDef {
	int   sprite;
	float scaleDev;
};

struct BoardTileDef {
	int sprite;
};

extern const SpriteDef    spriteDefs[NumSprites];
extern const BoardTileDef boardTileDefs[NumBoardTiles];
extern const GemDef       gemDefs[NumGemTypes];
extern const ObstacleDef  obstacleDefs[NumObstacleTypes];
extern const BoosterDef   boosterDefs[NumBoosters];
extern Wind::BitmapPtr    sprites[NumSprites];
