#include "TileSelector.h"
#include "Board.h"
#include "Config.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <engine/Input.h>

namespace {

constexpr int invalidIndex = -1;

std::pair<bool, int> GetCellAtCoordinates(const Board& board, const GameConfig& gameConfig, float x, float y) {
	const int col = static_cast<int>(std::floor((x - gameConfig.boardLeft) / gameConfig.cellWidthWithSpacing));
	const int row = static_cast<int>(std::floor((y - gameConfig.boardTop) / gameConfig.cellHeightWithSpacing));
	if (col < 0 || col >= board.GetCols() || row < 0 || row >= board.GetRows()) {
		return { false, invalidIndex };
	}
	const Cell& cell = board.GetCell(col, row);
	if (! IsSelectable(cell)) {
		return { false, invalidIndex };
	}
	return { true, board.GetCellIndex(col, row) };
}

} // namespace

enum class TileSelector::DragDirection {
	none,
	horizontal,
	vertical,
};

enum class TileSelector::State {
	none,
	firstSelected,
	waitForSecond,
};

TileSelector::TileSelector(Board& board, const GameConfig& gameConfig)
    : mBoard { board }
    , mGameConfig { gameConfig }
    , mState { State::none }
    , mSelectedCellIdx { invalidIndex }
    , mDragDirection { DragDirection::none }
    , mDragMouseCoords { 0.f, 0.f } {
}

void TileSelector::AddCallback(TileSelectionCallback&& cbk) {
	mCbk = std::move(cbk);
}

int TileSelector::GetSelectedCell() const {
	return mSelectedCellIdx;
}

void TileSelector::Reset() {
	SelectFirstCell(invalidIndex);
	mState = State::none;
	mDragDirection = DragDirection::none;
}

std::tuple<bool, int, int> TileSelector::SelectTiles(const Wind::Input& input) {
	const Wind::Vec2 mouseCoord = input.GetMappedMouseCoord();
	const bool  mouseButtonDown = input.GetMouseButtonDown();
	const bool  mouseButtonPressed = input.GetMouseButtonPressed();
	bool        res = false;
	int         first = 0;
	int         second = 0;
	if (mState == State::none) {
		// Wait for user to click on a cell
		if (mouseButtonPressed) {
			if (auto [hit, cellIdx] = GetCellAtCoordinates(mBoard, mGameConfig, mouseCoord.x, mouseCoord.y); hit) {
				mFirstCellIdx = cellIdx;
				StartDrag(mouseCoord.x, mouseCoord.y);
				mState = State::firstSelected;
			}
		}
	}
	else if (mState == State::firstSelected) {
		assert(mFirstCellIdx != invalidIndex);

		if (mouseButtonDown) {
			// Drag selected tile
			Cell& cell = mBoard.GetCell(mFirstCellIdx);

			const float deltaX = mouseCoord.x - mDragMouseCoords.x;
			const float deltaY = mouseCoord.y - mDragMouseCoords.y;
			// Start dragging the selected tile either horizontally or vertically after the mouse has moved over a threshold
			if (mDragDirection == DragDirection::none) {
				const float absDeltaX = std::abs(deltaX);
				const float absDeltaY = std::abs(deltaY);
				if (absDeltaX > mGameConfig.startDragThreshold || absDeltaY > mGameConfig.startDragThreshold) {
					mDragDirection = (absDeltaX > absDeltaY) ? DragDirection::horizontal : DragDirection::vertical;
				}
			}
			else if (mDragDirection == DragDirection::horizontal) {
				mDragMouseCoords.y = mouseCoord.y;
			}
			else if (mDragDirection == DragDirection::vertical) {
				mDragMouseCoords.x = mouseCoord.x;
			}

			// Wait for the selected tile to be dragged close enough to an adjacent tile, before attempting a swap
			int colDelta = 0;
			int rowDelta = 0;
			if (mDragDirection == DragDirection::horizontal) {
				if (DragTileX(cell, deltaX, mGameConfig.swapThreshold)) {
					colDelta = (deltaX > 0) ? 1 : -1;
				}
			}
			else if (mDragDirection == DragDirection::vertical) {
				if (DragTileY(cell, deltaY, mGameConfig.swapThreshold)) {
					rowDelta = (deltaY > 0) ? 1 : -1;
				}
			}

			if (rowDelta + colDelta) {
				first = mFirstCellIdx;
				second = mBoard.GetCellIndex(cell.col + colDelta, cell.row + rowDelta);
				const Cell& otherCell = mBoard.GetCell(second);
				if (IsSelectable(otherCell)) {
					res = true;
				}
			}
		}
		else {
			// Mouse button released. Undo dragging and wait for a second click
			SelectFirstCell(mFirstCellIdx);
			UndoDrag();
		}
	}
	else {
		// Wait for user to click on a second tile
		assert(mSelectedCellIdx != invalidIndex);
		if (mouseButtonPressed) {
			if (auto [hit, cellIdx] = GetCellAtCoordinates(mBoard, mGameConfig, mouseCoord.x, mouseCoord.y); hit) {
				if (cellIdx == mSelectedCellIdx) {
					SelectFirstCell(invalidIndex);
					mState = State::none;
				}
				else {
					res = true;
					first = mSelectedCellIdx;
					second = cellIdx;
				}
			}
		}
	}
	return { res, first, second };
}

void TileSelector::StartDrag(float mouseX, float mouseY) {
	mDragMouseCoords = { mouseX, mouseY };
	mDragDirection = DragDirection::none;
	TileSelectionEvent event;
	event.id = TileSelectionEvent::Id::drag;
	event.cellIdx = mFirstCellIdx;
	mCbk(event);
}

void TileSelector::SelectFirstCell(int idx) {
	if (mSelectedCellIdx != invalidIndex) {
		TileSelectionEvent event;
		event.id = TileSelectionEvent::Id::deselect;
		event.cellIdx = mSelectedCellIdx;
		mCbk(event);
	}
	if (idx != invalidIndex) {
		TileSelectionEvent event;
		event.id = TileSelectionEvent::Id::select;
		event.cellIdx = idx;
		mCbk(event);
	}
	mSelectedCellIdx = idx;
}

bool TileSelector::DragTileX(Cell& cell, float deltaX, float threshold) const {
	// Don't drag tile outside the board or further than one tile or against walls
	if (deltaX > 0 && ((cell.col == mBoard.GetCols() - 1) || ! IsSelectable(mBoard.GetCell(cell.col + 1, cell.row)))) {
		return false;
	}
	if (deltaX < 0 && ((cell.col == 0) || ! IsSelectable(mBoard.GetCell(cell.col - 1, cell.row)))) {
		return false;
	}
	cell.tileAnim.coords.x = cell.coords.x + std::clamp(deltaX, -mGameConfig.cellWidth, mGameConfig.cellWidth);
	return std::abs(cell.tileAnim.coords.x - cell.coords.x) > (threshold * mGameConfig.cellWidth);
}

bool TileSelector::DragTileY(Cell& cell, float deltaY, float threshold) const {
	// Don't drag tile outside the board or further than one tile or against walls
	if (deltaY > 0 && ((cell.row == mBoard.GetRows() - 1) || ! IsSelectable(mBoard.GetCell(cell.col, cell.row + 1)))) {
		return false;
	}
	if (deltaY < 0 && ((cell.row == 0 || ! IsSelectable(mBoard.GetCell(cell.col, cell.row - 1))))) {
		return false;
	}
	cell.tileAnim.coords.y = cell.coords.y + std::clamp(deltaY, -mGameConfig.cellHeight, mGameConfig.cellHeight);
	return std::abs(cell.tileAnim.coords.y - cell.coords.y) > (threshold * mGameConfig.cellHeight);
}

void TileSelector::UndoDrag() {
	assert(mSelectedCellIdx != invalidIndex);
	if (mDragDirection != DragDirection::none) {
		mDragDirection = DragDirection::none;
		TileSelectionEvent event;
		event.id = TileSelectionEvent::Id::undoDrag;
		event.cellIdx = mSelectedCellIdx;
		mCbk(event);
		Reset();
	}
	else {
		mState = State::waitForSecond;
	}
}
