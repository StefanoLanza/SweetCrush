#include "Board.h"
#include <cassert>

Board::Board(int cols, int rows)
    : mCells { static_cast<size_t>(cols * rows) }
    , mCols { cols }
    , mRows { rows }
    , mPieceCount {}
    , mTotalLayerCount { 0 } {
	assert(cols >= 1);
	assert(rows >= 1);
}

int Board::GetCols() const {
	return mCols;
}

int Board::GetRows() const {
	return mRows;
}

Wind::Span<Cell> Board::GetCells() {
	return { mCells.data(), mCells.size() };
}

Wind::Span<const Cell> Board::GetCells() const {
	return { mCells.data(), mCells.size() };
}

int Board::GetCellCount() const {
	return (int)mCells.size();
}

int Board::GetCellIndex(int col, int row) const {
	return col + mCols * row;
}

Cell& Board::GetCell(int index) {
	return mCells[index];
}

const Cell& Board::GetCell(int index) const {
	return mCells[index];
}

Cell& Board::GetCell(int col, int row) {
	assert(col >= 0 && col < mCols);
	assert(row >= 0 && row < mRows);
	return mCells[col + row * mCols];
}

const Cell& Board::GetCell(int col, int row) const {
	assert(col >= 0 && col < mCols);
	assert(row >= 0 && row < mRows);
	return mCells[col + row * mCols];
}

bool IsSelectable(const Cell& cell) {
	return (cell.category == CellCategory::piece && cell.layers == 0); // cannot select if frozen
}

bool Board::IsInside(int col, int row) const {
	return (col >= 0 && col < mCols && row >= 0 && row < mRows);
}

int Board::TotalLayerCount() const {
	int count = 0;
	for (const Cell& cell : mCells) {
		count += cell.layers;
	}
	return count;
}
