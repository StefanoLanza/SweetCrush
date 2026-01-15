#pragma once

#include "Effects.h"
#include <engine/FwdDecl.h>

constexpr int NumSprites = 25;
constexpr int NumPieceTypes = 11;
constexpr int NumBoardTiles = 1;
constexpr int NumObstacleTypes = 1;

struct SpriteDef {
	const char* bitmap;
};

extern Wind::TexturePtr sprites[NumSprites];
extern const SpriteDef  spriteDefs[NumSprites];
extern const int        boardTileDefs[NumBoardTiles];
extern const int        pieceIcons[NumPieceTypes];
extern const int        obstacleIcons[NumObstacleTypes];
extern const int        effectIcons[NumEffects];

extern const int sparkleSprite;
extern const int iceSprite;
extern const int brokenIceSprite;
extern const int selectionSprite;
extern const int glowSprite;
extern const int starSprite;
extern const int hrzStripesSprite;
extern const int vrtStripesSprite;