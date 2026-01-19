#pragma once

#include "Board.h"
#include <engine/Random.h>

class BoardGenerator final {
public:
	void GenRandomBoard(Board& board, uint32_t seed, const char* mask, const int pieceIds[], int numPieceIds);
	void InitBoard(Board& board, const char* boardDef, uint32_t seed, const int pieceIds[], int numPieceIds);
	void GenRandomPiece(Cell& cell, const Board& board);

private:
	static void ResetBoard(Board& board);

private:
	Wind::Random mRandomEngine;
	int          mPieceIds[8] {};
	int          mNumPieceIds = 0;
};