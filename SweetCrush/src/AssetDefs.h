#pragma once

#include "Effects.h"
#include <engine/FwdDecl.h>

using SpriteID = uint8_t;

constexpr int NumGameTextures = 20;
constexpr int NumPieceTypes = 9;
constexpr int NumBoardTiles = 1;
constexpr int NumObstacleTypes = 1;

extern Wind::TexturePtr gameTextures[NumGameTextures];
extern const char*      gameTexturePath[NumGameTextures];
extern const SpriteID   boardTileIcons[NumBoardTiles];
extern const SpriteID   pieceIcons[NumPieceTypes];
extern const SpriteID   obstacleIcons[NumObstacleTypes];
extern const SpriteID   effectIcons[NumEffects];

extern const SpriteID iceSprites[3];
extern const SpriteID selectionSprite;
extern const int glowSprite;
extern const int starSprite;
extern const int hrzStripesSprite;
extern const int vrtStripesSprite;
