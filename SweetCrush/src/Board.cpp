#include "Board.h"
#include <cassert>

Board::Board(int cols, int rows, const BoardConfig& cfg)
    : mCells { static_cast<size_t>(cols * rows) }
    , mCfg { cfg }
    , mCols { cols }
    , mRows { rows }
    , mPieceCount {}
    , mTotalLayerCount { 0 } {
	assert(cols >= 1);
	assert(rows >= 1);

	for (int row = 0; row < rows; ++row) {
		float y = row * cfg.cellHeightWithSpacing + cfg.topLeftCoord.y;
		for (int col = 0; col < cols; ++col) {
			float x = col * cfg.cellWidthWithSpacing + cfg.topLeftCoord.x;
			Cell& cell = GetCell(col, row);
			cell.coords = { x, y };
		}
	}
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

/*void Board::ReplaceCell(int index, const Cell& cell, void* ud) {
    mCells[index] = cell;
    mCells[index].ud = ud;
}

void Board::ReplaceCell(int col, int row, const Cell& cell, void* ud) {
    ReplaceCell(GetCellIndex(col,row), cell, ud);
}*/

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

bool IsSpecial(const Cell& cell) {
	return (cell.category == CellCategory::piece && cell.hasEffect);
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

int Board::GetCellAtCoords(Wind::Vec2 coords) const {
	const int col = static_cast<int>(std::floor((coords.x - mCfg.topLeftCoord.x) / mCfg.cellWidthWithSpacing));
	const int row = static_cast<int>(std::floor((coords.y - mCfg.topLeftCoord.y) / mCfg.cellHeightWithSpacing));
	if (col < 0 || col >= GetCols() || row < 0 || row >= GetRows()) {
		return -1;
	}
	return GetCellIndex(col, row);
}
