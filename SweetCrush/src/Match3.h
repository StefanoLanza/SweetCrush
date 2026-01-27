#pragma once

#include "Board.h"
#include <engine/FwdDecl.h>
#include <functional>
#include <vector>

enum class EffectType;

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
};

struct NewSpecialPieceEvent {
	EffectType  type;
	const Cell* cell;
	PieceId     pieceId;
};

struct NewPieceEvent {
	const Cell* cell;
	PieceId     pieceId;
};

struct RemovePieceEvent {
	const Cell* cell;
	int         targetCellIdx; // for suck anims
};

struct RemoveLayerEvent {
	const Cell* cell;
};

struct EffectEvent {
	const Cell* mainCell;
	EffectType  type;
};

struct Match3Event {
	enum class Id {
		swap,
		match,
		removePiece,
		newPiece,
		dropPiece,
		newEffect,
		triggerEffect,
		removeLayer,
	};
	Id id;
	union {
		MatchEvent           match;
		CellPairEvent        pair;
		NewPieceEvent        newPiece;
		NewSpecialPieceEvent specialPiece;
		EffectEvent          effect;
		RemovePieceEvent     removePiece;
		RemoveLayerEvent     removeLayer;
	};
};

using Match3Callback = std::function<void(const Match3Event& event)>;

class TileSelector;
class BoardGenerator;
struct AppConfig;

class Match3 final {
public:
	Match3(Board& board, BoardGenerator& boardGen, TileSelector& tileSelector);
	~Match3();

	void SetCallback(Match3Callback&& cbk);
	void ClearSelection();
	void Run();
	void UseBooster(int cellIdx);
	void Update(const Wind::Input& input);
	bool IsWaitingForUser() const;
	int  GetNumUserSwaps() const;

private:
	void DeleteAllPiecesOfType(int pieceId);
	bool SelectAndSwapPieces(const Wind::Input& input);
	bool CheckCombos(int h, int v, int t, int b, PieceId pieceId, int cellIdx);
	bool CheckCellCombos(int cellIdx);
	bool CheckMatchesAfterSwap();
	void KillCell(int cellIdx, int targetCellIdx);
	void CollapseColumns();
	void GenerateNewPieces();
	bool CheckMatches();
	bool TrySwap(int first, int second);
	void SwapSelectedCells(int firstTile, int secondTile);
	int  CollapseColumn(int col, CellPairEvent* collapseList);
	int  CollectMatches(int mainCellIdx, int deltaCol, int deltaRow, int* matches, int numMatches) const;
	void TriggerEffect(int cellIdx);
	// Boosters
	void HorizontalRocket(int col, int row);
	void VerticalRocket(int col, int row);
	void Bomb(int col, int row, int radius);

private:
	enum class State;

	Board&           mBoard;
	BoardGenerator&  mBoardGen;
	TileSelector&    mTileSelector;
	Match3Callback   mCbk;
	State            mState;
	CellPairEvent    mUserSwap;
	std::vector<int> mNewPieces;
	std::vector<int> mCheckList;
	int              mNumUserSwaps;
	int              mCascadeCount;
};
