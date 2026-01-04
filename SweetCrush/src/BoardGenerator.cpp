#include "BoardGenerator.h"

#include "Board.h"
#include "Constants.h"
#include "GameConfig.h"
#include "MatchChecker.h"
#include <engine/Random.h>

#include <cassert>

void BoardGenerator::GenRandomBoard(Board& board, uint32_t seed, const int pieceIds[], int numPieceIds, const BoardConfig& boardCfg) {
	mRandomEngine.Seed(seed);
	ResetBoard(board, boardCfg);

	assert((int)std::size(mPieceIds) >= numPieceIds);
	std::memcpy(mPieceIds, pieceIds, numPieceIds * sizeof pieceIds[0]);
	mNumPieceIds = numPieceIds;

	// Generate random pieces
	for (Cell& cell : board.GetCells()) {
		GenRandomPiece(cell, board);
	}
}

void BoardGenerator::InitBoard(Board& board, const char* boardDef, uint32_t seed, const int pieceIds[], int numPieceIds, const BoardConfig& boardCfg) {
	mRandomEngine.Seed(seed);
	ResetBoard(board, boardCfg);

	assert((int)std::size(mPieceIds) >= numPieceIds);
	std::memcpy(mPieceIds, pieceIds, numPieceIds * sizeof pieceIds[0]);
	mNumPieceIds = numPieceIds;

	for (int i = 0; i < board.GetCellCount(); ++i) {
		Cell& cell = board.GetCell(i);
		char  ch = boardDef[i];
		switch (boardDef[i]) {
		case holeCell:
			cell.category = CellCategory::hole;
			break;
		case obstacleCell:
			cell.category = CellCategory::obstacle;
			break;
		case emptyCell:
			cell.category = CellCategory::empty;
			break;
		default:
			cell.category = CellCategory::piece;
			if (ch >= 'A' && ch <= 'Z') {
				cell.pieceId = ch - 'A';
				cell.layers = 1;
			}
			else {
				cell.pieceId = ch - 'a';
				cell.layers = 0;
			}
			assert(cell.pieceId < MaxPieceTypes);
			break;
		}
	}
}

void BoardGenerator::ResetBoard(Board& board, const BoardConfig& boardCfg) {
	for (int row = 0; row < board.GetRows(); ++row) {
		float y = row * boardCfg.cellHeightWithSpacing + boardCfg.topLeftCoord.y;
		for (int col = 0; col < board.GetCols(); ++col) {
			float x = col * boardCfg.cellWidthWithSpacing + boardCfg.topLeftCoord.x;
			Cell& cell = board.GetCell(col, row);
			cell.coords = { x, y };
			cell.col = col;
			cell.row = row;
			cell.category = CellCategory::piece;
			cell.pieceId = 0;
			cell.layers = 0;
			cell.hasBooster = false;
		}
	}
}

void BoardGenerator::GenRandomPiece(Cell& cell, const Board& board) {
	assert(cell.category == CellCategory::piece);

	constexpr int maxAttempts = 100;
	bool          valid = false;
	int           attempts = 0;
	cell.layers = 0;
	cell.hasBooster = false;
	do {
		cell.pieceId = static_cast<PieceId>(mPieceIds[mRandomEngine.Next(0, mNumPieceIds - 1)]);
		// Avoid three or more consecutive matches
		valid = (1 + CountMatches(cell, board, CheckDirection::left) < 3) && (1 + CountMatches(cell, board, CheckDirection::top) < 3) &&
		        (1 + CountMatches(cell, board, CheckDirection::right) < 3) && (1 + CountMatches(cell, board, CheckDirection::bottom) < 3);
	} while (! valid && ++attempts < maxAttempts);
}
