#include "Match3.h"
#include "Board.h"
#include "BoardGenerator.h"
#include "Constants.h"
#include "MatchChecker.h"
#include "TileSelector.h"

#include <cassert>
#include <cstring>
#include <iterator> // std::size

// For debugging
#define ENABLE_EFFECTS    1
#define CREATE_NEW_PIECES 1

namespace {

void SwapCells(Board& board, int srcIdx, int dstIdx) {
	Cell& src = board.GetCell(srcIdx);
	Cell& dst = board.GetCell(dstIdx);
	std::swap(src.category, dst.category);
	std::swap(src.pieceId, dst.pieceId);
	std::swap(src.layers, dst.layers);
	std::swap(src.effect, dst.effect);
}

//  OOO
//   O
//   O
bool T3Combo(int l, int r, int t, int b) {
	return (l == 1 && r == 1 && t == 2 && b == 0) || //
	       (l == 1 && r == 1 && b == 2 && t == 0) || //
	       (t == 1 && b == 1 && l == 2 && r == 0) || //
	       (t == 1 && b == 1 && r == 2 && l == 0);
}

//  OOOO
//   O
//   O
bool T4Combo(int l, int r, int t, int b) {
	return ((l + r == 3) && t == 2 && b == 0) || //
	       ((l + r == 3) && b == 2 && t == 0) || //
	       ((t + b == 3) && l == 2 && r == 0) || //
	       ((t + b == 3) && r == 2 && l == 0);
}

// OOOOO
//   O
//   O
bool T5Combo(int l, int r, int t, int b) {
	return ((l + r == 4) && t == 2 && b == 0) || //
	       ((l + r == 4) && b == 2 && t == 0) || //
	       ((t + b == 4) && l == 2 && r == 0) || //
	       ((t + b == 4) && r == 2 && l == 0);
}

// OOO
//   O
//   O
bool LCombo(int l, int r, int t, int b) {
	return (l == 2 && t == 2 && (r + b) == 0) || //
	       (l == 2 && b == 2 && (r + t) == 0) || //
	       (r == 2 && b == 2 && (l + t) == 0) || //
	       (r == 2 && t == 2 && (l + b) == 0);
}

bool _5Combo(int l, int r, int t, int b) {
	int h = l + r + 1;
	int v = t + b + 1;
	return ((h == 5) && (v < 3)) || //
	       ((v == 5) && (h < 3));
}

bool _4Combo(int l, int r, int t, int b) {
	int h = l + r + 1;
	int v = t + b + 1;
	return ((h == 4) && (v < 3)) || //
	       ((v == 4) && (h < 3));
}

bool _3Combo(int l, int r, int t, int b) {
	int h = l + r + 1;
	int v = t + b + 1;
	return ((h == 3) && (v < 3)) || //
	       ((v == 3) && (h < 3));
}

bool _AnyCombo(int l, int r, int t, int b) {
	int h = l + r + 1;
	int v = t + b + 1;
	return (h >= 3) || (v >= 3);
}

} // namespace

enum class Match3::State {
	selectAndSwapPieces,
	checkSwap,
	collapseColumns,
	checkMatches
};

Match3::Match3(Board& board, BoardGenerator& boardGen, TileSelector& tileSelector)
    : mBoard { board }
    , mBoardGen { boardGen }
    , mTileSelector { tileSelector }
    , mState { State::selectAndSwapPieces }
    , mUserSwap { 0, 0 }
    , mCascadeCount { 0 } {
}

Match3::~Match3() = default;

void Match3::SetClientCallback(Match3Callback&& cbk) {
	mCbk = std::move(cbk);
}

void Match3::ClearSelection() {
	mTileSelector.Reset();
}

void Match3::Restart() {
	// Clear lists of previous match
	mNewPieces.clear();
	mTileSelector.Reset();
	mState = State::selectAndSwapPieces;
	mCascadeCount = 0;
}

void Match3::UseBooster(int cellIdx) {
	assert(mState == State::selectAndSwapPieces);
	// TODO handle more booster types
	KillCell(mBoard.GetCell(cellIdx), nullptr);
	mState = State::collapseColumns;
}

void Match3::Update(const Wind::Input& input) {
	switch (mState) {
	case State::selectAndSwapPieces:
		if (SelectAndSwapPieces(input)) {
			mState = State::checkSwap;
		}
		// else keeping waiting for user selection
		break;
	case State::checkSwap:
		if (CheckSpecialComboAfterSwap()) {
			mState = State::collapseColumns;
		}
		else if (CheckMatchesAfterSwap()) {
			mState = State::collapseColumns;
		}
		else {
			// No matches or special combos, undo swap
			Match3Event event;
			event.id = Match3Event::Id::undoSwap;
			event.pair = mUserSwap;
			mCbk(event);

			SwapCells(mBoard, mUserSwap.first, mUserSwap.second);
			mState = State::selectAndSwapPieces;
		}
		break;
	case State::collapseColumns:
		CollapseColumns();
		GenerateNewPieces();
		// Wait for drop animations before checking matches
		mState = State::checkMatches;
		break;
	case State::checkMatches:
		if (CheckMatches()) {
			// Cascade
			mState = State::collapseColumns;
		}
		else {
			// Back to user selection
			mCascadeCount = 0;
			mState = State::selectAndSwapPieces;
		}
		break;
	default:
		assert(false);
		break;
	};
}

bool Match3::IsWaitingForUser() const {
	return mState == State::selectAndSwapPieces;
}

bool Match3::SelectAndSwapPieces(const Wind::Input& input) {
	if (auto [swap, first, second] = mTileSelector.SelectTiles(input); swap) {
		mTileSelector.Reset();
		return TrySwap(first, second);
	}
	return false;
}

bool Match3::CheckCombos(int l, int r, int t, int b, PieceId pieceId, int mainCellIdx) {
	bool       res = true;
	ComboType  comboType {};
	bool       horizontalMatch = false;
	EffectType effectType {};

	// TODO 2x2 square ?

	if (T3Combo(l, r, t, b)) {
		comboType = ComboType::T3;
		effectType = EffectType::wrapped;
	}
	else if (T4Combo(l, r, t, b)) {
		comboType = ComboType::T4;
		effectType = EffectType::wrapped;
	}
	else if (T5Combo(l, r, t, b)) {
		comboType = ComboType::T5;
		effectType = EffectType::colorBomb;
	}
	else if (LCombo(l, r, t, b)) {
		comboType = ComboType::L;
		effectType = EffectType::wrapped;
	}
	else if (_5Combo(l, r, t, b)) {
		comboType = ComboType::C5;
		horizontalMatch = (l + r + 1) == 5;
		effectType = EffectType::colorBomb;
	}
	else if (_4Combo(l, r, t, b)) {
		comboType = ComboType::C4;
		horizontalMatch = (l + r + 1) == 4;
		effectType = horizontalMatch ? EffectType::hStriped : EffectType::vStriped;
	}
	else if (_3Combo(l, r, t, b)) {
		comboType = ComboType::C3;
		horizontalMatch = (l + r + 1) == 3;
	}
	else if (_AnyCombo(l, r, t, b)) {
		comboType = ComboType::Unknown;
	}
	else {
		res = false;
	}

	Cell& mainCell = mBoard.GetCell(mainCellIdx);

	if (res) {
		// Inform client
		Match3Event event;
		event.id = Match3Event::Id::match;
		event.match.comboType = comboType;
		event.match.pieceId = pieceId;
		event.match.cell = &mainCell;
		event.match.cascadeCount = mCascadeCount;
		mCbk(event);

		int matches[NumRows * NumCols];
		int numMatches = 0;
		if (1 + t + b >= 3) {
			// Kill vertical matches
			numMatches = CollectMatches(mainCellIdx, 0, -1, matches, numMatches);
			numMatches = CollectMatches(mainCellIdx, 0, +1, matches, numMatches);
		}
		if (1 + l + r >= 3) {
			// Kill horizontal matches
			numMatches = CollectMatches(mainCellIdx, -1, 0, matches, numMatches);
			numMatches = CollectMatches(mainCellIdx, +1, 0, matches, numMatches);
		}
		assert(numMatches < NumRows * NumCols);

#if ENABLE_EFFECTS
		bool makeSpecialCandy = (comboType != ComboType::C3);
		// In Candy Crush, if a special candy appears in a combo, it is activated and NO new special candy is created
		for (int i = 0; i < numMatches; ++i) {
			if (IsSpecial(mBoard.GetCell(matches[i]))) {
				makeSpecialCandy = false;
				break;
			}
		}
		if (IsSpecial(mainCell)) {
			makeSpecialCandy = false;
		}

		// If any candy in the matches is caged/frozen, do *not* create a special candy. Only remove the layer
		for (int i = 0; i < numMatches; ++i) {
			if (mBoard.GetCell(matches[i]).layers > 0) {
				makeSpecialCandy = false;
				break;
			}
		}
#else
		const bool isSpecialCombo = false;
#endif

		if (makeSpecialCandy) {
			// Special combo. Associate an effect with the piece in the main cell
			assert(mainCell.category == CellCategory::piece);
			assert(mainCell.layers == 0);
			mainCell.effect = effectType;

			// Inform client
			event.id = Match3Event::Id::newEffect;
			event.specialPiece.cell = &mainCell;
			event.specialPiece.pieceId = mainCell.pieceId; // FIXME redundant ?
			event.specialPiece.type = effectType;
			mCbk(event);
		}
		else {
			// Kill main piece along with its matches
			KillCell(mainCell, nullptr);
		}

		for (int i = 0; i < numMatches; ++i) {
			KillCell(mBoard.GetCell(matches[i]), makeSpecialCandy ? &mainCell : nullptr);
		}
	}

	return res;
}

bool Match3::CheckCellCombos(int cellIdx) {
	const Cell& cell = mBoard.GetCell(cellIdx);
	if (cell.category != CellCategory::piece) {
		return false; // already deleted
	}
	const int l = CountMatches(cell, mBoard, CheckDirection::left);
	const int r = CountMatches(cell, mBoard, CheckDirection::right);
	const int t = CountMatches(cell, mBoard, CheckDirection::top);
	const int b = CountMatches(cell, mBoard, CheckDirection::bottom);
	return CheckCombos(l, r, t, b, cell.pieceId, cellIdx);
}

bool Match3::CheckMatchesAfterSwap() {
	// Run both checks !
	bool res = CheckCellCombos(mUserSwap.second);
	res = CheckCellCombos(mUserSwap.first) || res;
	return res;
}

bool Match3::CheckSpecialCombo(int firstIdx, int secondIdx) {
	Cell& firstCell = mBoard.GetCell(firstIdx);
	Cell& secondCell = mBoard.GetCell(secondIdx);
	assert(firstCell.category == CellCategory::piece);
	assert(secondCell.category == CellCategory::piece);

	if (firstCell.effect == EffectType::colorBomb && secondCell.effect == EffectType::none) {
		// Every single candy of that color is removed from the entire board
		ColorBomb(firstCell, secondCell.pieceId);
		return true;
	}
	else if (firstCell.effect == EffectType::colorBomb && secondCell.effect == EffectType::colorBomb) {
		ClearBoard();
		return true;
	}
	else if (firstCell.effect == EffectType::colorBomb) {
		if (secondCell.effect == EffectType::hStriped || secondCell.effect == EffectType::vStriped) {
			// TODO Transform all candies of secondCell.pieceId into a striped, then activate them
		}
	}
	else if (firstCell.effect == EffectType::colorBomb && secondCell.effect == EffectType::wrapped) {
		// TODO
		// 1 eleminate all candies of secondCell.pieceId
		// 2 Immediately after those candies disappear, the Color Bomb chooses the next most common color currently on the board and removes all of
		// those as well.
	}

	// TODO Replace with a table
	else if (firstCell.effect == EffectType::hStriped && secondCell.effect == EffectType::hStriped) {
		// hStriped + vStriped effect
	}
	else if (firstCell.effect == EffectType::vStriped && secondCell.effect == EffectType::vStriped) {
		// hStriped + vStriped effect
	}
	else if (firstCell.effect == EffectType::hStriped && secondCell.effect == EffectType::wrapped) {
		// TODO
	}
	else if (firstCell.effect == EffectType::hStriped && secondCell.effect == EffectType::vStriped) {
	}
	else {
		return false; // not a valid combo
	}

	return false; //FIXME true;
}

bool Match3::CheckSpecialComboAfterSwap() {
	if (CheckSpecialCombo(mUserSwap.first, mUserSwap.second)) {
		return true;
	}
	if (CheckSpecialCombo(mUserSwap.second, mUserSwap.first)) {
		return true;
	}
	return false;
}

void Match3::KillCell(Cell& cell, const Cell* targetCell) {
	if (cell.category != CellCategory::piece) {
		return; // already deleted or hole or obstacle
	}
	if (cell.layers == 0) {
		if (cell.effect != EffectType::none) {
			TriggerEffect(cell);
		}
		else {
			// Inform client
			Match3Event event {
				.id = Match3Event::Id::removePiece,
				.removePiece = { .cell = &cell, .targetCell = targetCell },
			};
			mCbk(event);

			cell.category = CellCategory::empty;
		}
	}
	else {
		// Remove one layer
		--cell.layers;
		// Inform client
		Match3Event event {
			.id = Match3Event::Id::removeLayer,
			.removeLayer = { .cell = &cell },
		};
		mCbk(event);
	}
}

void Match3::CollapseColumns() {
	CellPairEvent collapseList[NumRows];
	for (int col = 0; col < mBoard.GetCols(); ++col) {
		int numCollapsed = CollapseColumn(col, collapseList);
		for (int i = 0; i < numCollapsed; ++i) {
			SwapCells(mBoard, collapseList[i].first, collapseList[i].second);

			// Inform client
			Match3Event event {
				.id = Match3Event::Id::dropPiece,
				.pair = collapseList[i],
			};
			mCbk(event);
		}
	}
}

void Match3::GenerateNewPieces() {
#if CREATE_NEW_PIECES
	for (int cellIdx : mNewPieces) {
		Cell& cell = mBoard.GetCell(cellIdx);
		assert(cell.category == CellCategory::empty);
		cell.category = CellCategory::piece;
		mBoardGen.GenRandomPiece(cell, mBoard);

		// Check new pieces. In theory the random generator should not generate pieces that create matches
		mCheckList.push_back(cellIdx);

		// Inform client
		Match3Event event {
			.id = Match3Event::Id::newPiece,
			.newPiece = { .cell = &cell, .pieceId = cell.pieceId },
		};
		mCbk(event);
	}
#endif
	mNewPieces.clear();
}

bool Match3::CheckMatches() {
	// TODO This could be optimized for huge boards by marking invalid columns and rows when generating new cells
	++mCascadeCount;
	bool res = false;
	for (int c : mCheckList) {
		res = CheckCellCombos(c) || res;
	}
	mCheckList.clear();
	return res;
}

bool Match3::TrySwap(int first, int second) {
	bool        res = false;
	const Cell& firstCell = mBoard.GetCell(first);
	const Cell& secondCell = mBoard.GetCell(second);
	// Check whether the first and second tiles are adjacent and inside the board
	if (firstCell.col >= 0 && firstCell.col == secondCell.col - 1 && firstCell.row == secondCell.row) {
		SwapSelectedCells(first, second);
		res = true;
	}
	else if (firstCell.col < mBoard.GetCols() && firstCell.col == secondCell.col + 1 && firstCell.row == secondCell.row) {
		SwapSelectedCells(second, first);
		res = true;
	}
	else if (firstCell.row >= 0 && firstCell.row == secondCell.row - 1 && firstCell.col == secondCell.col) {
		SwapSelectedCells(first, second);
		res = true;
	}
	else if (firstCell.row < mBoard.GetRows() && firstCell.row == secondCell.row + 1 && firstCell.col == secondCell.col) {
		SwapSelectedCells(second, first);
		res = true;
	}
	return res;
}

void Match3::SwapSelectedCells(int firstTile, int secondTile) {
	mUserSwap.first = firstTile;
	mUserSwap.second = secondTile;

	Match3Event event {
		.id = Match3Event::Id::swap,
		.pair = { firstTile, secondTile },
	};
	mCbk(event);

	SwapCells(mBoard, firstTile, secondTile);
}

int Match3::CollapseColumn(int col, CellPairEvent* collapseList) {
	int emptyRows[NumRows];
	int numEmptyRows = 0;
	int currEmptyRow = 0;
	int numCollapsed = 0;

	// From bottom to top, stack active tiles
	for (int row = mBoard.GetRows() - 1; row >= 0; --row) {
		const int   src = mBoard.GetCellIndex(col, row);
		const Cell& cell = mBoard.GetCell(src);
		switch (cell.category) {
		case CellCategory::empty:
			emptyRows[numEmptyRows++] = row;
			break;
		case CellCategory::obstacle:
			numEmptyRows = 0; // block pieces above
			currEmptyRow = 0;
			break;
		case CellCategory::piece:
			if (currEmptyRow < numEmptyRows) {
				// Fall to an empty mainCell
				const int dst = mBoard.GetCellIndex(col, emptyRows[currEmptyRow]);
				++currEmptyRow;
				collapseList[numCollapsed] = { src, dst };
				++numCollapsed;

				// Check matches after piece has been dropped to dst mainCell
				mCheckList.push_back(dst);

				emptyRows[numEmptyRows++] = row;
			}
			break;
		case CellCategory::hole:
			// skip it
			break;
		case CellCategory::star:
			// skip it
			break;
		default:
			assert(false);
			break;
		}
	}

	// Drop new pieces on the remaining empty rows
	for (int e = currEmptyRow; e < numEmptyRows; ++e) {
		mNewPieces.push_back(mBoard.GetCellIndex(col, emptyRows[e]));
	}

	return numCollapsed;
}

int Match3::CollectMatches(int mainCellIdx, int deltaCol, int deltaRow, int* matches, int numMatches) const {
	const Cell& cell = mBoard.GetCell(mainCellIdx);
	int         col = cell.col + deltaCol;
	int         row = cell.row + deltaRow;
	while (col >= 0 && col < mBoard.GetCols() && row >= 0 && row < mBoard.GetRows()) {
		const int cellIdx = mBoard.GetCellIndex(col, row);
		assert(cellIdx != mainCellIdx); // maincell handled separately
		const Cell& otherCell = mBoard.GetCell(cellIdx);
		if (CheckMatch(otherCell, cell)) {
			matches[numMatches] = cellIdx;
			++numMatches;
		}
		else {
			break;
		}
		col += deltaCol;
		row += deltaRow;
	}
	return numMatches;
}

void Match3::TriggerEffect(Cell& cell) {
	assert(cell.category == CellCategory::piece);
	assert(cell.effect != EffectType::none);
	assert(cell.layers == 0);

	switch (cell.effect) {
    case EffectType::none:
        break;
	case EffectType::hStriped:
		DeleteRow(cell);
        break;
	case EffectType::vStriped:
		DeleteColumn(cell);
        break;
	case EffectType::wrapped:
		Bomb(cell, 1);
		break;
	case EffectType::colorBomb:
		// Color bomb hit by a blast. Choose most frequent candy in board
		ColorBomb(cell, FindMostFrequentPiece());
		break;
	}
}

void Match3::DeleteRow(Cell& mainCell) {
	// Inform client e.g. to play some special fx
	Match3Event event {
		.id = Match3Event::Id::triggerEffect,
		.effect = { .mainCell = &mainCell, .type = EffectType::hStriped },
	};
	mCbk(event);

	// Delete special candy
	// Important: do it before triggering, to avoid infinite recursion
	mainCell.category = CellCategory::empty;
	mainCell.effect = EffectType::none;

	for (int ncol = 0; ncol < mBoard.GetCols(); ++ncol) {
		if (mainCell.col != ncol) {
			KillCell(mBoard.GetCell(ncol, mainCell.row));
		}
	}
}

void Match3::DeleteColumn(Cell& mainCell) {
	// Inform client e.g. to play some special fx
	Match3Event event {
		.id = Match3Event::Id::triggerEffect,
		.effect = { .mainCell = &mainCell, .type = EffectType::vStriped },
	};
	mCbk(event);

	// Delete special candy
	// Important: do it before triggering, to avoid infinite recursion
	mainCell.category = CellCategory::empty;
	mainCell.effect = EffectType::none;

	for (int nrow = 0; nrow < mBoard.GetRows(); ++nrow) {
		if (mainCell.row != nrow) {
			KillCell(mBoard.GetCell(mainCell.col, nrow));
		}
	}
}

void Match3::Bomb(Cell& mainCell, int radius) {
	// Inform client e.g. to play some special fx
	Match3Event event {
		.id = Match3Event::Id::triggerEffect,
		.effect = { .mainCell = &mainCell, .type = EffectType::wrapped },
	};
	mCbk(event);

	// Delete special candy
	// Important: do it before triggering, to avoid infinite recursion
	mainCell.category = CellCategory::empty;
	mainCell.effect = EffectType::none;

	// Kill grid around bomb
	for (int y = -radius; y <= radius; ++y) {
		int orow = mainCell.row + y;
		for (int x = -radius; x <= radius; ++x) {
			int r = x * x + y * y;
			if (r > 0) {
				int ocol = mainCell.col + x;
				if (mBoard.IsInside(ocol, orow)) {
					KillCell(mBoard.GetCell(ocol, orow));
				}
			}
		}
	}
}

void Match3::ColorBomb(Cell& mainCell, PieceId targetPieceId) {
	// Inform client e.g. to play some special fx
	Match3Event event {
		.id = Match3Event::Id::triggerEffect,
		.effect = { .mainCell = &mainCell, .type = EffectType::colorBomb },
	};
	mCbk(event);

	// Delete special candy
	// Important: do it before triggering, to avoid infinite recursion
	mainCell.category = CellCategory::empty;
	mainCell.effect = EffectType::none;

	for (Cell& cell : mBoard.GetCells()) {
		if (cell.category == CellCategory::piece && cell.pieceId == targetPieceId) {
			KillCell(cell);
		}
	}
}

void Match3::ClearBoard() {
	// TODO Are layers removed ?
	for (Cell& cell : mBoard.GetCells()) {
		if (cell.category == CellCategory::piece) {
			KillCell(cell);
		}
	}
}

PieceId Match3::FindMostFrequentPiece() const {
	int     count[MaxPieceTypes] {};
	PieceId pieceId = 0;
	int     highestCount = 0;
	for (const Cell& cell : mBoard.GetCells()) {
		if (cell.category == CellCategory::piece) {
			assert(cell.pieceId < MaxPieceTypes);
			count[cell.pieceId]++;
			if (count[cell.pieceId] > highestCount) {
				pieceId = cell.pieceId;
			}
		}
	}
	return pieceId;
}
