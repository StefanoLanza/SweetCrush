#include "Match3.h"
#include "Board.h"
#include "Boosters.h"
#include "Constants.h"
#include "GameConfig.h"
#include "TileSelector.h"
#include <cassert>
#include <cstring>
#include <engine/Input.h>

namespace {

bool CheckMatch(const Cell& a, const Cell& b) {
	return (a.category == b.category) && (a.category != CellCategory::empty) && (a.pieceId == b.pieceId);
}

int CountMatches(const Cell& cell, const Board& board, Direction dir) {
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

void GenRandomPiece(Cell& cell, const Board& board, Wind::Random& random, const int gemIds[], int numGemTypes) {
	assert(cell.category == CellCategory::piece);
	bool          valid = false;
	constexpr int maxAttempts = 100;
	int           attempts = 0;
	cell.hits = 1;
	do {
		cell.pieceId = static_cast<PieceId>(gemIds[random.Next(0, numGemTypes - 1)]);
		// Avoid three or more consecutive matches
		valid = (1 + CountMatches(cell, board, Direction::left) < 3) && (1 + CountMatches(cell, board, Direction::top) < 3) &&
		        (1 + CountMatches(cell, board, Direction::right) < 3) && (1 + CountMatches(cell, board, Direction::bottom) < 3);
	} while (! valid && ++attempts < maxAttempts);
}

void SwapTiles(Board& board, int srcIdx, int dstIdx) {
	Cell& src = board.GetCell(srcIdx);
	Cell& dst = board.GetCell(dstIdx);
	std::swap(src.pieceId, dst.pieceId);
	std::swap(src.hits, dst.hits);
	std::swap(src.category, dst.category);
	std::swap(dst.pieceAnim, src.pieceAnim);
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
	return ((h > 4) && (v < 3)) || //
	       ((v > 4) && (h < 3));
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
	selectTiles,
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
    , mState { State::selectTiles }
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
			cell.hits = 1;
		}
	}

	if (boardDef) {
		for (int i = 0; i < mBoard.GetCellCount(); ++i) {
			Cell& cell = mBoard.GetCell(i);
			switch (boardDef[i]) {
			case holeCell:
				cell.category = CellCategory::hole;
				cell.backgroundTileIdx = 0; // no background
				break;
			case obstacleCell:
				cell.category = CellCategory::obstacle;
				break;
			case emptyCell:
				cell.category = CellCategory::empty;
				break;
			default:
				cell.category = CellCategory::piece;
				cell.pieceId = boardDef[i] - '0';
				assert(cell.pieceId < MaxPieceTypes);
				break;
			}
		}
	}
	else {
		// Generate random pieces
		for (Cell& cell : mBoard.GetCells()) {
			GenRandomPiece(cell, mBoard, mRandomEngine, gemIds, numGemIds);
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
	mState = State::selectTiles;
	mNumUserSwaps = 0;
	mCascadeCount = 0;
}

void Match3::Update(const Wind::Input& input) {
	for (const CellPair& s : mSwaps) {
		SwapTiles(mBoard, s.first, s.second);
	}
	mSwaps.clear();

	for (const CellPair& pair : mCollapseList) {
		SwapTiles(mBoard, pair.first, pair.second);
	}
	mCollapseList.clear();

	switch (mState) {
	case State::selectTiles:
		SelectTiles(input);
		break;
	case State::checkMatchesAfterSwap:
		if (CheckMatchesAfterSwap()) {
			mState = State::insertBoosters;
		}
		else {
			// No matches, undo swap
			mSwaps.push_back(mUserSwap);
			mState = State::selectTiles;
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
			mState = State::selectTiles;
		}
		break;
	default:
		assert(false);
		break;
	};

	for (const CellPair& pair : mSwaps) {
		Match3Event event;
		event.id = Match3Event::Id::swap;
		event.pair = pair;
		mCbk(event);
	}
}

int Match3::GetNumUserSwaps() const {
	return mNumUserSwaps;
}

void Match3::SelectTiles(const Wind::Input& input) {
	if (int selected = mTileSelector.GetSelectedCell(); selected > 0) {
		const Cell& cell = mBoard.GetCell(selected);
		if (HasBooster(cell)) {
			TriggerBooster(selected);
			mTileSelector.Reset();
			return;
		}
	}

	if (auto [swap, first, second] = mTileSelector.SelectTiles(input); swap) {
		TrySwap(first, second);
		mTileSelector.Reset();
	}
}

void Match3::AddBooster(BoosterType boosterType, int cellIdx) {
	mNewBoosters.push_back({ boosterType, cellIdx });
}

void Match3::HorizontalRocket(int col, int row) {
	for (int ncol = 0; ncol < mBoard.GetCols(); ++ncol) {
		int cellIdx = mBoard.GetCellIndex(ncol, row);
		if (HasPiece(mBoard.GetCell(cellIdx))) {
			RemoveTile(cellIdx);
		}
	}
	RemoveTile(mBoard.GetCellIndex(col, row)); // remove booster
}

void Match3::VerticalRocket(int col, int row) {
	for (int nrow = 0; nrow < mBoard.GetRows(); ++nrow) {
		int cellIdx = mBoard.GetCellIndex(col, nrow);
		if (HasPiece(mBoard.GetCell(cellIdx))) {
			RemoveTile(cellIdx);
		}
	}
	RemoveTile(mBoard.GetCellIndex(col, row)); // remove booster
}

void Match3::Bomb(int col, int row, int radius) {
	for (int y = -radius; y <= radius; ++y) {
		int orow = row + y;
		for (int x = -radius; x <= radius; ++x) {
			if (x * x + y * y <= radius * radius) { // within radius
				int ocol = col + x;
				if (mBoard.IsInside(ocol, orow)) {
					int cellIdx = mBoard.GetCellIndex(ocol, orow);
					if (HasPiece(mBoard.GetCell(cellIdx))) {
						RemoveTile(cellIdx);
					}
				}
			}
		}
	}
	RemoveTile(mBoard.GetCellIndex(col, row)); // remove booster
}

void Match3::DeleteAllPieces(int pieceId) {
	int cellIdx = 0;
	for (const Cell& cell : mBoard.GetCells()) {
		if (HasPiece(cell) && cell.pieceId == pieceId) {
			RemoveTile(cellIdx);
		}
		++cellIdx;
	}
}

bool Match3::CheckCombos(int l, int r, int t, int b, PieceId pieceId, int cellIdx) {
	bool        res = true;
	Match3Event event;

	if (T3Combo(l, r, t, b)) {
		event.match.comboType = ComboType::T3;
	}
	else if (T4Combo(l, r, t, b)) {
		event.match.comboType = ComboType::T4;
	}
	else if (T5Combo(l, r, t, b)) {
		event.match.comboType = ComboType::T5;
	}
	else if (LCombo(l, r, t, b)) {
		event.match.comboType = ComboType::L;
	}
	else if (_5Combo(l, r, t, b)) {
		event.match.comboType = ComboType::C5;
	}
	else if (_4Combo(l, r, t, b)) {
		event.match.comboType = ComboType::C4;
	}
	else if (_3Combo(l, r, t, b)) {
		event.match.comboType = ComboType::C3;
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
	const int   l = CountMatches(cell, mBoard, Direction::left);
	const int   r = CountMatches(cell, mBoard, Direction::right);
	const int   t = CountMatches(cell, mBoard, Direction::top);
	const int   b = CountMatches(cell, mBoard, Direction::bottom);
	const bool  res = CheckCombos(l, r, t, b, cell.pieceId, cellIdx);

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
		RemoveTile(cellIdx);
	}

	return res;
}

bool Match3::CheckMatchesAfterSwap() {
	bool res = CheckCellCombos(mUserSwap.first);
	res = CheckCellCombos(mUserSwap.second) || res;
	return res;
}

void Match3::RemoveTile(int idx) const {
	// Inform client
	Match3Event event;
	event.id = Match3Event::Id::removeTile;
	event.cellIdx = idx;
	mCbk(event);

	Cell& cell = mBoard.GetCell(idx);
	cell.category = CellCategory::empty;
	cell.pieceId = 255;
	cell.pieceAnim.spriteIdx = -1;
}

void Match3::InsertBoosters() {
	for (const Booster& booster : mNewBoosters) {
		// Replace tile
		Cell& cell = mBoard.GetCell(booster.cellIdx);
		cell.category = CellCategory::booster;
		cell.pieceId = static_cast<PieceId>(booster.type);
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
	for (const CellPair& pair : mCollapseList) {
		Match3Event event;
		event.id = Match3Event::Id::dropTile;
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
		GenRandomPiece(cell, mBoard, mRandomEngine, mGemIds, mNumGemIds);
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
	// TODO This could be optimized for huge boards by marking invalid columns and rows when generating new tiles
	++mCascadeCount;
	bool res = false;
	for (int c : mCheckList) {
		res = CheckCellCombos(c) || res;
	}
	mCheckList.clear();
	return res;
}

void Match3::TrySwap(int first, int second) {
	const Cell& firstTile = mBoard.GetCell(first);
	const Cell& secondTile = mBoard.GetCell(second);
	// Check whether the first and second tiles are adjacent and inside the board
	if (firstTile.col >= 0 && firstTile.col == secondTile.col - 1 && firstTile.row == secondTile.row) {
		SwapSelectedTiles(first, second);
	}
	else if (firstTile.col < mBoard.GetCols() && firstTile.col == secondTile.col + 1 && firstTile.row == secondTile.row) {
		SwapSelectedTiles(second, first);
	}
	else if (firstTile.row >= 0 && firstTile.row == secondTile.row - 1 && firstTile.col == secondTile.col) {
		SwapSelectedTiles(first, second);
	}
	else if (firstTile.row < mBoard.GetRows() && firstTile.row == secondTile.row + 1 && firstTile.col == secondTile.col) {
		SwapSelectedTiles(second, first);
	}
}

void Match3::SwapSelectedTiles(int firstTile, int secondTile) {
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
		if (IsEmpty(cell)) {
			emptyRows[numEmptyRows++] = row;
		}
		else if (HasObstacle(cell)) {
			numEmptyRows = 0; // block pieces and boosts above
			currEmptyRow = 0;
		}
		else { // booster or piece
			if (currEmptyRow < numEmptyRows) {
				// Fall to an empty cell
				const int dst = mBoard.GetCellIndex(col, emptyRows[currEmptyRow]);
				++currEmptyRow;
				mCollapseList.push_back({ src, dst });

				mCheckList.push_back(dst);

				emptyRows[numEmptyRows++] = row;
			}
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
		const int cellIdx = mBoard.GetCellIndex(col, row);
		Cell&     otherCell = mBoard.GetCell(cellIdx);
		if (CheckMatch(otherCell, cell)) {
			if (--otherCell.hits == 0) {
				RemoveTile(cellIdx);
			}
		}
		else {
			break;
		}
		col += dcol;
		row += drow;
	}
}

void Match3::TriggerBooster(int cellIdx) {
	const Cell& cell = mBoard.GetCell(cellIdx);
	assert(cell.category == CellCategory::booster);
	const BoosterType boosterType = static_cast<BoosterType>(cell.pieceId);

	// Inform client
	Match3Event event;
	event.id = Match3Event::Id::boosterTriggered;
	event.booster.cellIdx = cellIdx;
	event.booster.type = boosterType;
	mCbk(event);

	mState = State::collapseColumns;
}
