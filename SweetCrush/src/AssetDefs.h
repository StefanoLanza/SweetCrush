#pragma once

#include "Board.h"
#include <engine/FwdDecl.h>

constexpr int NumGemTypes = 9;
constexpr int NumBoardTiles = 1;
constexpr int NumSprites = 15;
constexpr int NumObstacleTypes = 1;
constexpr int NumUIBitmaps = 2;

extern const SpriteDef    spriteDefs[NumSprites];
extern const BoardTileDef boardTileDefs[NumBoardTiles];
extern const GemDef       gemDefs[NumGemTypes];
extern const ObstacleDef  obstacleDefs[NumObstacleTypes];
extern const BoosterDef   boosterDefs[NumBoosters];
extern Wind::BitmapPtr    sprites[NumSprites];

extern Wind::BitmapPtr UIBitmaps[NumUIBitmaps];
