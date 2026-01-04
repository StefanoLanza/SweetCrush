#pragma once

#include "Board.h"
#include <engine/FwdDecl.h>
#include <functional>
#include <vector>

enum class BoosterType;

struct CellPairEvent {
	int first;
	int second;
};

enum class ComboType {
	C3,
	C4,
	C5,
	T3,
	T4,
	T5,
	L,
	Unknown
};

struct MatchEvent {
	ComboType comboType;
	PieceId   pieceId;
	int       cellIdx;
	int       cascadeCount;
	bool      horizontal;
};

struct BoosterEvent {
	BoosterType type;
	int         cellIdx;
	PieceId     pieceId;
};

struct NewPieceEvent {
	int     cellIdx;
	PieceId targetPieceId;
};

struct CellEvent {
	int cellIdx;
};

struct Match3Event {
	enum class Id {
		swap,
		match,
		removePiece,
		newPiece,
		dropPiece,
		newBooster,
		triggerBooster,
		removeLayer,
	};
	Id id;
	union {
		MatchEvent    match;
		CellPairEvent pair;
		NewPieceEvent newPiece;
		BoosterEvent  booster;
		CellEvent     removePiece;
		CellEvent     removeLayer;
	};
};

using Match3Callback = std::function<void(const Match3Event& event)>;

class TileSelector;
class BoardGenerator;
struct GameConfig;

class Match3 final {
public:
	Match3(Board& board, BoardGenerator& boardGen, const GameConfig& gameConfig, TileSelector& tileSelector);
	~Match3();

	void SetCallback(Match3Callback&& cbk);
	void Run();
	void Update(const Wind::Input& input);
	int  GetNumUserSwaps() const;

private:
	void AddBooster(BoosterType boosterType, int cellIdx, PieceId pieceId);
	// Boosters
	void HorizontalRocket(int col, int row);
	void VerticalRocket(int col, int row);
	void Bomb(int col, int row, int radius);
	void DeleteAllPiecesOfType(int pieceId);
	// States
	void SelectPieces(const Wind::Input& input);
	bool CheckCombos(int h, int v, int t, int b, PieceId pieceId, int cellIdx);
	bool CheckCellCombos(int cellIdx);
	bool CheckMatchesAfterSwap();
	void KillCell(int cellIdx);
	void InsertBoosters();
	void CollapseColumns();
	void GenerateNewPieces();
	bool CheckMatches();

	void TrySwap(int first, int second);
	void SwapSelectedCells(int firstTile, int secondTile);
	void CollapseColumn(int col);
	void KillMatches(const Cell& cell, int dcol, int drow);
	void TriggerBooster(int cellIdx);

private:
	enum class State;

	Board&                     mBoard;
	BoardGenerator&            mBoardGen;
	TileSelector&              mTileSelector;
	const GameConfig&          mGameConfig;
	Match3Callback             mCbk;
	State                      mState;
	CellPairEvent              mUserSwap;
	std::vector<CellPairEvent> mSwaps;
	std::vector<int>           mNewPieces;
	std::vector<int>           mCheckList;
	std::vector<CellPairEvent> mCollapseList;
	std::vector<BoosterEvent>  mNewBoosters;
	int                        mNumUserSwaps;
	int                        mCascadeCount;
};
