#include "MatchChecker.h"

#include "Board.h"

#include <cassert>

bool CheckMatch(const Cell& a, const Cell& b) {
	return (a.category == b.category) && (a.pieceId == b.pieceId);
}

int CountMatches(const Cell& cell, const Board& board, CheckDirection dir) {
	assert(cell.category == CellCategory::piece);

	int dcol = 0;
	int drow = 0;
	int iter = 0;
	switch (dir) {
	case CheckDirection::left:
		dcol = -1;
		drow = 0;
		iter = cell.col;
		break;
	case CheckDirection::right:
		dcol = +1;
		drow = 0;
		iter = board.GetCols() - 1 - cell.col;
		break;
	case CheckDirection::top:
		dcol = 0;
		drow = -1;
		iter = cell.row;
		break;
	case CheckDirection::bottom:
		dcol = 0;
		drow = +1;
		iter = board.GetRows() - 1 - cell.row;
		break;
	}
	int matches = 0;
	int col = cell.col + dcol;
	int row = cell.row + drow;
	for (int i = 0; i < iter; ++i, ++matches) {
		const Cell& otherCell = board.GetCell(col, row);
		if (! CheckMatch(otherCell, cell)) {
			break;
		}
		col += dcol;
		row += drow;
	}
	return matches;
}
