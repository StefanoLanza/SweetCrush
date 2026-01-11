#pragma once

#include <engine/FwdDecl.h>

class Board;
struct GameConfig;

void DrawBoardTiles(const Board& board, int selectedCell, const Wind::BitmapRenderer& bitmapRender, const GameConfig& gameConfig);
