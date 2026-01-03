#include "Match3.h"
#include "Board.h"
#include "Constants.h"
#include "GameConfig.h"
#include "TileSelector.h"

#include <engine/Input.h>

#include <cassert>
#include <cstring>
#include <iterator> // std::size

namespace {

bool CheckMatch(const Cell& a, const Cell& b) {
	return (a.category == b.category) && (a.pieceId == b.pieceId);
}

int CountMatches(const Cell& cell, const Board& board, Direction dir) {
	assert(cell.category == CellCategory::piece);

	int dcol = 0;
	int drow = 0;
	int iter = 0;
	switch (dir) {
	case Direction::left:
		dcol = -1;
		drow = 0;
		iter = cell.col;
		break;
	case Direction::right:
		dcol = +1;
		drow = 0;
		iter = board.GetCols() - 1 - cell.col;
		break;
	case Direction::top:
		dcol = 0;
		drow = -1;
		iter = cell.row;
		break;
	case Direction::bottom:
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

void GenRandomPiece(Cell& cell, const Board& board, Wind::Random& random, const int gemIds[], int numGemTypes, const Match3Config& cfg) {
	assert(cell.category == CellCategory::piece);

	constexpr int maxAttempts = 100;
	bool          valid = false;
	int           attempts = 0;
	cell.layers = 0;
	cell.hasBooster = false;
	do {
		cell.pieceId = static_cast<PieceId>(gemIds[random.Next(0, numGemTypes - 1)]);
		// Avoid three or more consecutive matches
		valid = (1 + CountMatches(cell, board, Direction::left) < 3) && (1 + CountMatches(cell, board, Direction::top) < 3) &&
		        (1 + CountMatches(cell, board, Direction::right) < 3) && (1 + CountMatches(cell, board, Direction::bottom) < 3);
	} while (! valid && ++attempts < maxAttempts);
}

void SwapCells(Board& board, int srcIdx, int dstIdx) {
	Cell& src = board.GetCell(srcIdx);
	Cell& dst = board.GetCell(dstIdx);
	std::swap(src.category, dst.category);
	std::swap(src.pieceId, dst.pieceId);
	std::swap(src.layers, dst.layers);
	std::swap(dst.pieceAnim, src.pieceAnim);
	std::swap(src.hasBooster, dst.hasBooster);
	std::swap(src.boosterType, dst.boosterType);
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
	selectPieces,
	checkMatchesAfterSwap,
	collapseColumns,
	generateNewPieces,
	checkMatches,
	insertBoosters,
};

Match3::Match3(Board& board, const GameConfig& gameConfig, TileSelector& tileSelector)
    : mTileSelector { tileSelector }
    , mBoard { board }
    , mGameConfig { gameConfig }
    , mGemIds {}
    , mNumGemIds { 0 }
    , mState { State::selectPieces }
    , mUserSwap { 0, 0 }
    , mNumUserSwaps { 0 }
    , mCascadeCount { 0 } {
}

Match3::~Match3() = default;

void Match3::SetCallback(Match3Callback&& cbk) {
	mCbk = std::move(cbk);
}

void Match3::NewBoard(uint32_t seed, const char* boardDef, const int gemIds[], int numGemIds) {
	assert(numGemIds > 0);

	mRandomEngine.Seed(seed);

	// Reset board first
	for (int row = 0; row < mBoard.GetRows(); ++row) {
		float y = row * mGameConfig.cellHeightWithSpacing + mGameConfig.boardTop;
		for (int col = 0; col < mBoard.GetCols(); ++col) {
			float x = col * mGameConfig.cellWidthWithSpacing + mGameConfig.boardLeft;
			Cell& cell = mBoard.GetCell(col, row);
			cell.coords = { x, y };
			cell.col = col;
			cell.row = row;
			cell.category = CellCategory::piece;
			cell.pieceId = 0;
			cell.backgroundTileIdx = 1;
			cell.layers = 0;
			cell.hasBooster = false;
		}
	}

	if (boardDef) {
		for (int i = 0; i < mBoard.GetCellCount(); ++i) {
			Cell& cell = mBoard.GetCell(i);
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
	else {
		// Generate random pieces
		for (Cell& cell : mBoard.GetCells()) {
			GenRandomPiece(cell, mBoard, mRandomEngine, gemIds, numGemIds, mGameConfig.match3);
		}
	}

	assert((int)std::size(mGemIds) >= numGemIds);
	std::memcpy(mGemIds, gemIds, numGemIds * sizeof gemIds[0]);
	mNumGemIds = numGemIds;
}

void Match3::Run() {
	// Clear lists from previous match
	mSwaps.clear();
	mNewPieces.clear();
	mCollapseList.clear();
	mNewBoosters.clear();

	mTileSelector.Reset();
	mState = State::selectPieces;
	mNumUserSwaps = 0;
	mCascadeCount = 0;
}

void Match3::Update(const Wind::Input& input) {
	for (const CellPairEvent& s : mSwaps) {
		SwapCells(mBoard, s.first, s.second);
	}
	mSwaps.clear();

	for (const CellPairEvent& pair : mCollapseList) {
		SwapCells(mBoard, pair.first, pair.second);
	}
	mCollapseList.clear();

	switch (mState) {
	case State::selectPieces:
		SelectPieces(input);
		break;
	case State::checkMatchesAfterSwap:
		if (CheckMatchesAfterSwap()) {
			mState = State::insertBoosters;
		}
		else {
			// No matches, undo swap
			mSwaps.push_back(mUserSwap);
			mState = State::selectPieces;
		}
		break;
	case State::insertBoosters:
		InsertBoosters();
		mState = State::collapseColumns;
		break;
	case State::collapseColumns:
		CollapseColumns();
		mState = State::generateNewPieces;
		break;
	case State::generateNewPieces:
		GenerateNewPieces();
		mState = State::checkMatches;
		break;
	case State::checkMatches:
		if (CheckMatches()) {
			mState = State::insertBoosters;
		}
		else {
			mCascadeCount = 0;
			mState = State::selectPieces;
		}
		break;
	default:
		assert(false);
		break;
	};

	for (const CellPairEvent& pair : mSwaps) {
		Match3Event event;
		event.id = Match3Event::Id::swap;
		event.pair = pair;
		mCbk(event);
	}
}

int Match3::GetNumUserSwaps() const {
	return mNumUserSwaps;
}

void Match3::SelectPieces(const Wind::Input& input) {
	if (auto [swap, first, second] = mTileSelector.SelectTiles(input); swap) {
		TrySwap(first, second);
		mTileSelector.Reset();
	}
}

void Match3::AddBooster(BoosterType boosterType, int cellIdx, PieceId pieceId) {
	mNewBoosters.push_back({ boosterType, cellIdx, pieceId });
}

void Match3::HorizontalRocket(int col, int row) {
	// Kill entire row
	for (int ncol = 0; ncol < mBoard.GetCols(); ++ncol) {
		if (col != ncol) {
			int cellIdx = mBoard.GetCellIndex(ncol, row);
			KillCell(cellIdx);
		}
	}
}

void Match3::VerticalRocket(int col, int row) {
	// Kill entire column
	for (int nrow = 0; nrow < mBoard.GetRows(); ++nrow) {
		if (row != nrow) {
			int cellIdx = mBoard.GetCellIndex(col, nrow);
			KillCell(cellIdx);
		}
	}
}

void Match3::Bomb(int col, int row, int radius) {
	// Kill grid around bomb
	for (int y = -radius; y <= radius; ++y) {
		int orow = row + y;
		for (int x = -radius; x <= radius; ++x) {
			if (x * x + y * y > 0) {
				int ocol = col + x;
				if (mBoard.IsInside(ocol, orow)) {
					int cellIdx = mBoard.GetCellIndex(ocol, orow);
					KillCell(cellIdx);
				}
			}
		}
	}
}

void Match3::DeleteAllPiecesOfType(int pieceId) {
	int cellIdx = 0;
	for (const Cell& cell : mBoard.GetCells()) {
		if (cell.category == CellCategory::piece && cell.pieceId == pieceId) {
			KillCell(cellIdx);
		}
		++cellIdx;
	}
}

bool Match3::CheckCombos(int l, int r, int t, int b, PieceId pieceId, int cellIdx) {
	bool        res = true;
	Match3Event event {};

	// TODO Distinguish horizontal and vertical for T and L combos ?

	if (T3Combo(l, r, t, b)) {
		event.match.comboType = ComboType::T3;
		AddBooster(BoosterType::vrocket, cellIdx, pieceId);
	}
	else if (T4Combo(l, r, t, b)) {
		event.match.comboType = ComboType::T4;
		AddBooster(BoosterType::bomb, cellIdx, pieceId);
	}
	else if (T5Combo(l, r, t, b)) {
		event.match.comboType = ComboType::T5;
		AddBooster(BoosterType::bomb, cellIdx, pieceId);
	}
	else if (LCombo(l, r, t, b)) {
		event.match.comboType = ComboType::L;
		AddBooster(BoosterType::bomb, cellIdx, pieceId);
	}
	else if (_5Combo(l, r, t, b)) {
		event.match.comboType = ComboType::C5;
		AddBooster(BoosterType::hrocket, cellIdx, pieceId);
		event.match.horizontal = (l + r + 1) == 5;
	}
	else if (_4Combo(l, r, t, b)) {
		event.match.comboType = ComboType::C4;
		AddBooster(BoosterType::miniBomb, cellIdx, pieceId);
		event.match.horizontal = (l + r + 1) == 4;
	}
	else if (_3Combo(l, r, t, b)) {
		event.match.comboType = ComboType::C3;
		event.match.horizontal = (l + r + 1) == 3;
	}
	else if (_AnyCombo(l, r, t, b)) {
		event.match.comboType = ComboType::Unknown;
	}
	else {
		res = false;
	}

	if (res) {
		// Inform client
		event.id = Match3Event::Id::match;
		event.match.pieceId = pieceId;
		event.match.cascadeCount = mCascadeCount;
		event.match.cellIdx = cellIdx;
		mCbk(event);
	}

	return res;
}

bool Match3::CheckCellCombos(int cellIdx) {
	const Cell& cell = mBoard.GetCell(cellIdx);
	if (cell.category != CellCategory::piece) {
		return false; // already deleted
	}

	const int  l = CountMatches(cell, mBoard, Direction::left);
	const int  r = CountMatches(cell, mBoard, Direction::right);
	const int  t = CountMatches(cell, mBoard, Direction::top);
	const int  b = CountMatches(cell, mBoard, Direction::bottom);
	const bool res = CheckCombos(l, r, t, b, cell.pieceId, cellIdx);

	if (res) {
		if (1 + t + b >= 3) {
			// Kill vertical matches
			KillMatches(cell, 0, -1);
			KillMatches(cell, 0, +1);
		}
		if (1 + l + r >= 3) {
			// Kill horizontal matches
			KillMatches(cell, -1, 0);
			KillMatches(cell, +1, 0);
		}
		// Kill main cell
		KillCell(cellIdx);
	}

	return res;
}

bool Match3::CheckMatchesAfterSwap() {
	bool res = CheckCellCombos(mUserSwap.first);
	res = CheckCellCombos(mUserSwap.second) || res;
	return res;
}

void Match3::KillCell(int idx) {
	Cell& cell = mBoard.GetCell(idx);
	if (cell.category == CellCategory::piece) {
		if (cell.layers == 0) {
			if (cell.hasBooster) {
				TriggerBooster(idx);
			}
			else {
				// Inform client
				Match3Event event;
				event.id = Match3Event::Id::removePiece;
				event.removePiece.cellIdx = idx;
				mCbk(event);

				cell.category = CellCategory::empty;
				cell.hasBooster = false;
			}
		}
		else {
			// Remove one layer
			--cell.layers;
			// Inform client
			Match3Event event;
			event.id = Match3Event::Id::removeLayer;
			event.removeLayer.cellIdx = idx;
			mCbk(event);
		}
	}
}

void Match3::InsertBoosters() {
	for (const BoosterEvent& booster : mNewBoosters) {
		Cell& cell = mBoard.GetCell(booster.cellIdx);
		assert(cell.category == CellCategory::empty); // must have been deleted
		cell.category = CellCategory::piece;
		cell.pieceId = booster.pieceId;
		cell.hasBooster = true;
		cell.boosterType = booster.type;
		cell.layers = 0;

		// Inform client
		Match3Event event;
		event.id = Match3Event::Id::newBooster;
		event.booster = booster;
		mCbk(event);
	}
	mNewBoosters.clear();
}

void Match3::CollapseColumns() {
	mCollapseList.clear();
	for (int col = 0; col < mBoard.GetCols(); ++col) {
		CollapseColumn(col);
	}

	// Inform client
	for (const CellPairEvent& pair : mCollapseList) {
		Match3Event event;
		event.id = Match3Event::Id::dropPiece;
		event.pair = pair;
		mCbk(event);
	}

	mCheckList.insert(std::end(mCheckList), std::begin(mNewPieces), std::end(mNewPieces));
	// mCheckList.insert(std::end(mCheckList), std::begin(mCollapseList), std::end(mCollapseList));
}

void Match3::GenerateNewPieces() {
	for (int cellIdx : mNewPieces) {
		Cell& cell = mBoard.GetCell(cellIdx);
		assert(IsEmpty(cell));
		cell.category = CellCategory::piece;
		GenRandomPiece(cell, mBoard, mRandomEngine, mGemIds, mNumGemIds, mGameConfig.match3);

		// Inform client
		Match3Event event;
		event.id = Match3Event::Id::newPiece;
		event.newPiece.cellIdx = cellIdx;
		event.newPiece.targetPieceId = cell.pieceId;
		mCbk(event);
	}
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

void Match3::TrySwap(int first, int second) {
	const Cell& firstCell = mBoard.GetCell(first);
	const Cell& secondCell = mBoard.GetCell(second);
	// Check whether the first and second tiles are adjacent and inside the board
	if (firstCell.col >= 0 && firstCell.col == secondCell.col - 1 && firstCell.row == secondCell.row) {
		SwapSelectedCells(first, second);
	}
	else if (firstCell.col < mBoard.GetCols() && firstCell.col == secondCell.col + 1 && firstCell.row == secondCell.row) {
		SwapSelectedCells(second, first);
	}
	else if (firstCell.row >= 0 && firstCell.row == secondCell.row - 1 && firstCell.col == secondCell.col) {
		SwapSelectedCells(first, second);
	}
	else if (firstCell.row < mBoard.GetRows() && firstCell.row == secondCell.row + 1 && firstCell.col == secondCell.col) {
		SwapSelectedCells(second, first);
	}
}

void Match3::SwapSelectedCells(int firstTile, int secondTile) {
	mUserSwap.first = firstTile;
	mUserSwap.second = secondTile;
	mSwaps.push_back(mUserSwap);
	mState = State::checkMatchesAfterSwap;
	mNumUserSwaps++;
}

void Match3::CollapseColumn(int col) {
	int emptyRows[NumRows];
	int numEmptyRows = 0;
	int currEmptyRow = 0;

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
				// Fall to an empty cell
				const int dst = mBoard.GetCellIndex(col, emptyRows[currEmptyRow]);
				++currEmptyRow;
				mCollapseList.push_back({ src, dst });

				mCheckList.push_back(dst);

				emptyRows[numEmptyRows++] = row;
			}
			break;
		case CellCategory::hole:
			// skip it
			break;
		}
	}

	for (int e = currEmptyRow; e < numEmptyRows; ++e) {
		mNewPieces.push_back(mBoard.GetCellIndex(col, emptyRows[e]));
	}
}

void Match3::KillMatches(const Cell& cell, int dcol, int drow) {
	int col = cell.col + dcol;
	int row = cell.row + drow;
	while (col >= 0 && col < mBoard.GetCols() && row >= 0 && row < mBoard.GetRows()) {
		const int   cellIdx = mBoard.GetCellIndex(col, row);
		const Cell& otherCell = mBoard.GetCell(cellIdx);
		if (CheckMatch(otherCell, cell)) {
			KillCell(cellIdx);
		}
		else {
			break;
		}
		col += dcol;
		row += drow;
	}
}

void Match3::TriggerBooster(int cellIdx) {
	Cell& cell = mBoard.GetCell(cellIdx);
	assert(cell.category == CellCategory::piece);
	assert(cell.hasBooster);
	assert(cell.layers == 0);

	// Inform client
	Match3Event event;
	event.id = Match3Event::Id::triggerBooster;
	event.booster.cellIdx = cellIdx;
	event.booster.type = cell.boosterType;
	mCbk(event);

	// Delete piece with booster
	// Important: do it before triggering, to avoid infinite recursion in same cases
	cell.category = CellCategory::empty;
	cell.hasBooster = false;

	switch (cell.boosterType) {
	case BoosterType::hrocket: {
		HorizontalRocket(cell.col, cell.row);
	} break;
	case BoosterType::vrocket: {
		VerticalRocket(cell.col, cell.row);
	} break;
	case BoosterType::miniBomb:
		Bomb(cell.col, cell.row, 1);
		break;
	case BoosterType::bomb:
		Bomb(cell.col, cell.row, 2);
		break;
	default:
		break;
	}
}
