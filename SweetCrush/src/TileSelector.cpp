#include "TileSelector.h"
#include "AppConfig.h"
#include "Board.h"

#include <engine/Input.h>

#include <algorithm>
#include <cassert>
#include <cmath>

using namespace Wind;

namespace {

constexpr int invalidIndex = -1;

std::pair<bool, int> GetCellAtCoordinates(const Board& board, Vec2 coords) {
	int cellIdx = board.GetCellAtCoords(coords);
	if (cellIdx < 0) {
		return { false, invalidIndex };
	}
	const Cell& cell = board.GetCell(cellIdx);
	if (! IsSelectable(cell)) {
		return { false, invalidIndex };
	}
	return { true, cellIdx };
}

} // namespace

enum class TileSelector::DragDirection {
	empty,
	horizontal,
	vertical,
};

enum class TileSelector::State {
	empty,
	firstSelected,
	waitForSecond,
};

TileSelector::TileSelector(const Board& board, const AppConfig& gameConfig)
    : mBoard { board }
    , mAppConfig { gameConfig }
    , mState { State::empty }
    , mFirstCellIdx { invalidIndex }
    , mSelectedCellIdx { invalidIndex }
    , mDragDirection { DragDirection::empty }
    , mDragMouseCoords { 0.f, 0.f } {
}

void TileSelector::SetCallback(TileSelectionCallback&& cbk) {
	mCbk = std::move(cbk);
}

int TileSelector::GetSelectedTile() const {
	return mSelectedCellIdx;
}

void TileSelector::Reset() {
	SelectFirstCell(invalidIndex);
	mState = State::empty;
	mDragDirection = DragDirection::empty;
}

std::tuple<bool, int, int> TileSelector::SelectTiles(const Input& input) {
	const Vec2 mouseCoord = input.GetMappedMouseCoord();
	const bool       mouseButtonDown = input.GetMouseButtonDown(MouseButton::left);
	const bool       mouseButtonPressed = input.GetMouseButtonPressed(MouseButton::left);
	bool             res = false;
	int              first = 0;
	int              second = 0;
	if (mState == State::empty) {
		// Wait for user to click on a cell
		if (mouseButtonPressed) {
			if (auto [hit, cellIdx] = GetCellAtCoordinates(mBoard, mouseCoord); hit) {
				mFirstCellIdx = cellIdx;
				StartDrag(mouseCoord.x, mouseCoord.y);
				mState = State::firstSelected;
			}
		}
	}
	else if (mState == State::firstSelected) {
		assert(mFirstCellIdx != invalidIndex);

		if (mouseButtonDown) {
			const float deltaX = mouseCoord.x - mDragMouseCoords.x;
			const float deltaY = mouseCoord.y - mDragMouseCoords.y;
			// Start dragging the selected tile either horizontally or vertically after the mouse has moved over a threshold
			if (mDragDirection == DragDirection::empty) {
				const float absDeltaX = std::abs(deltaX);
				const float absDeltaY = std::abs(deltaY);
				if (absDeltaX > mAppConfig.ui.startDragThreshold || absDeltaY > mAppConfig.ui.startDragThreshold) {
					mDragDirection = (absDeltaX > absDeltaY) ? DragDirection::horizontal : DragDirection::vertical;
				}
			}
			else if (mDragDirection == DragDirection::horizontal) {
				mDragMouseCoords.y = mouseCoord.y;
			}
			else if (mDragDirection == DragDirection::vertical) {
				mDragMouseCoords.x = mouseCoord.x;
			}

			// Drag selected tile
			const Cell& cell = mBoard.GetCell(mFirstCellIdx);

			// Wait for the selected tile to be dragged close enough to an adjacent tile, before attempting a swap
			int colDelta = 0;
			int rowDelta = 0;
			if (mDragDirection == DragDirection::horizontal) {
				if (DragTileX(cell, deltaX, mAppConfig.ui.swapThreshold)) {
					colDelta = (deltaX > 0) ? 1 : -1;
				}
			}
			else if (mDragDirection == DragDirection::vertical) {
				if (DragTileY(cell, deltaY, mAppConfig.ui.swapThreshold)) {
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
			if (auto [hit, cellIdx] = GetCellAtCoordinates(mBoard, mouseCoord); hit) {
				if (cellIdx == mSelectedCellIdx) {
					SelectFirstCell(invalidIndex);
					mState = State::empty;
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
	mDragDirection = DragDirection::empty;
}

void TileSelector::SelectFirstCell(int idx) {
#if 0
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
#endif
	mSelectedCellIdx = idx;
}

bool TileSelector::DragTileX(const Cell& cell, float deltaX, float threshold) const {
	// Don't drag tile outside the board or further than one tile or against non-selectables
	if (deltaX > 0 && ((cell.col == mBoard.GetCols() - 1) || ! IsSelectable(mBoard.GetCell(cell.col + 1, cell.row)))) {
		return false;
	}
	if (deltaX < 0 && ((cell.col == 0) || ! IsSelectable(mBoard.GetCell(cell.col - 1, cell.row)))) {
		return false;
	}

	float xCoord = cell.coords.x + std::clamp(deltaX, -mAppConfig.board.cellWidth, mAppConfig.board.cellWidth);
	;
	xCoord = cell.coords.x + std::clamp(deltaX, -mAppConfig.board.cellWidth, mAppConfig.board.cellWidth);

	TileSelectionEvent event;
	event.id = TileSelectionEvent::Id::drag;
	event.cell = &cell;
	event.draggedCoord = { xCoord, cell.coords.y };
	mCbk(event);

	return std::abs(xCoord - cell.coords.x) > (threshold * mAppConfig.board.cellWidth);
}

bool TileSelector::DragTileY(const Cell& cell, float deltaY, float threshold) const {
	// Don't drag tile outside the board or further than one tile or against non-selectables
	if (deltaY > 0 && ((cell.row == mBoard.GetRows() - 1) || ! IsSelectable(mBoard.GetCell(cell.col, cell.row + 1)))) {
		return false;
	}
	if (deltaY < 0 && ((cell.row == 0 || ! IsSelectable(mBoard.GetCell(cell.col, cell.row - 1))))) {
		return false;
	}
	float yCoord = cell.coords.y + std::clamp(deltaY, -mAppConfig.board.cellHeight, mAppConfig.board.cellHeight);

	TileSelectionEvent event;
	event.id = TileSelectionEvent::Id::drag;
	event.cell = &cell;
	event.draggedCoord = { cell.coords.x, yCoord };
	mCbk(event);

	return std::abs(yCoord - cell.coords.y) > (threshold * mAppConfig.board.cellHeight);
}

void TileSelector::UndoDrag() {
	assert(mSelectedCellIdx != invalidIndex);
	if (mDragDirection != DragDirection::empty) {
		mDragDirection = DragDirection::empty;
		TileSelectionEvent event;
		event.id = TileSelectionEvent::Id::undoDrag;
		event.cell = &mBoard.GetCell(mSelectedCellIdx);
		mCbk(event);
		Reset();
	}
	else {
		mState = State::waitForSecond;
	}
}
