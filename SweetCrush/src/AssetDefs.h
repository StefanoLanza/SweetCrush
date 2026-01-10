#pragma once

#include "Boosters.h"
#include <engine/FwdDecl.h>

constexpr int NumSprites = 23;
constexpr int NumPieceTypes = 11;
constexpr int NumBoardTiles = 1;
constexpr int NumObstacleTypes = 1;

struct SpriteDef {
	const char* bitmap;
};

struct PieceDef {
	int sprite;
};

struct ObstacleDef {
	int sprite;
};

struct BoosterDef {
	int sprite;
};

struct BoardTileDef {
	int sprite;
};

extern const SpriteDef    spriteDefs[NumSprites];
extern const BoardTileDef boardTileDefs[NumBoardTiles];
extern const PieceDef     pieceDefs[NumPieceTypes];
extern const ObstacleDef  obstacleDefs[NumObstacleTypes];
extern const BoosterDef   boosterDefs[NumBoosters];
extern Wind::TexturePtr   sprites[NumSprites];

extern const int sparkleSprite;
extern const int iceSprite;
extern const int brokenIceSprite;
extern const int selectionSprite;
extern const int glowSprite;
