#include "BoardGenerator.h"

#include "Board.h"
#include "Constants.h"
#include "AppConfig.h"
#include "MatchChecker.h"
#include <engine/Random.h>

#include <cassert>

void BoardGenerator::GenRandomBoard(Board& board, uint32_t seed, const char* mask, const int pieceIds[], int numPieceIds,
                                    const BoardConfig& boardCfg) {
	mRandomEngine.Seed(seed);
	ResetBoard(board, boardCfg);

	assert((int)std::size(mPieceIds) >= numPieceIds);
	std::memcpy(mPieceIds, pieceIds, numPieceIds * sizeof pieceIds[0]);
	mNumPieceIds = numPieceIds;

	auto cells = board.GetCells();
	if (mask) {
		for (size_t i = 0; i < cells.Size(); ++i) {
			if (mask[i] != ' ') {
				GenRandomPiece(cells[i], board);
			}
			else {
				cells[i].category = CellCategory::hole;
			}
		}
	}
	else {
		for (Cell& cell : cells) {
			GenRandomPiece(cell, board);
		}
	}
}

void BoardGenerator::InitBoard(Board& board, const char* boardDef, uint32_t seed, const int pieceIds[], int numPieceIds,
                               const BoardConfig& boardCfg) {
	mRandomEngine.Seed(seed);
	ResetBoard(board, boardCfg);

	assert((int)std::size(mPieceIds) >= numPieceIds);
	std::memcpy(mPieceIds, pieceIds, numPieceIds * sizeof pieceIds[0]);
	mNumPieceIds = numPieceIds;

	for (int i = 0; i < board.GetCellCount(); ++i) {
		Cell& cell = board.GetCell(i);
		char  ch = boardDef[i];
		switch (boardDef[i]) {
		case holeChar:
			cell.category = CellCategory::hole;
			break;
		case obstacleChar:
			cell.category = CellCategory::obstacle;
			break;
		case emptyChar:
			cell.category = CellCategory::empty;
			break;
		case starChar:
			cell.category = CellCategory::star;
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
			Cell&  cell = board.GetCell(col, row);
			cell.coords = { x, y };
			cell.col = col;
			cell.row = row;
			cell.category = CellCategory::piece;
			cell.pieceId = 0;
			cell.layers = 0;
			cell.hasEffect = false;
			//board.ReplaceCell(col, row, cell, cell.ud);
		}
	}
}

void BoardGenerator::GenRandomPiece(Cell& cell, const Board& board) {
	assert(cell.category == CellCategory::piece);

	constexpr int maxAttempts = 100;
	bool          valid = false;
	int           attempts = 0;
	cell.layers = 0;
	cell.hasEffect = false;
	do {
		cell.pieceId = static_cast<PieceId>(mPieceIds[mRandomEngine.Next(0, mNumPieceIds - 1)]);
		// Avoid three or more consecutive matches
		valid = (1 + CountMatches(cell, board, CheckDirection::left) < 3) && (1 + CountMatches(cell, board, CheckDirection::top) < 3) &&
		        (1 + CountMatches(cell, board, CheckDirection::right) < 3) && (1 + CountMatches(cell, board, CheckDirection::bottom) < 3);
	} while (! valid && ++attempts < maxAttempts);
}
