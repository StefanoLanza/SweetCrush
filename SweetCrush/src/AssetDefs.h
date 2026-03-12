#pragma once

#include "Effects.h"
#include <engine/FwdDecl.h>

using SpriteID = uint8_t;

constexpr int NumGameTextures = 23;
constexpr int NumPieceTypes = 9;
constexpr int NumBoardTiles = 1;
constexpr int NumObstacleTypes = 1;
constexpr int NumBoosters = 3;

extern const char*      gameTexturePath[NumGameTextures];
extern Wind::TexturePtr gameTextures[NumGameTextures];
extern const char*      gameTexturePath[NumGameTextures];
extern const SpriteID   boardTileIcons[NumBoardTiles];
extern const SpriteID   pieceIcons[NumPieceTypes];
extern const SpriteID   obstacleIcons[NumObstacleTypes];
extern const SpriteID   boosterIcons[NumBoosters];

extern const SpriteID iceSprites[3];
extern const SpriteID selectionSprite;
extern const SpriteID glowSprite;
extern const SpriteID blastSprite;
extern const SpriteID starSprite;
extern const SpriteID colorBombIcon;
