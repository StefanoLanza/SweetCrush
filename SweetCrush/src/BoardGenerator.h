#pragma once

#include "Board.h"
#include <engine/Random.h>

struct BoardConfig;

class BoardGenerator final {
public:
	void GenRandomBoard(Board& board, uint32_t seed, const int pieceIds[], int numPieceIds, const BoardConfig& boardCfg);
	void InitBoard(Board& board, const char* boardDef, uint32_t seed, const int pieceIds[], int numPieceIds, const BoardConfig& boardCfg);
	void GenRandomPiece(Cell& cell, const Board& board);

private:
	static void ResetBoard(Board& board, const BoardConfig& boardCfg);

private:
	Wind::Random mRandomEngine;
	int          mPieceIds[8] {};
	int          mNumPieceIds = 0;
};