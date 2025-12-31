#pragma once

#include "Board.h"
#include <engine/FwdDecl.h>
#include <engine/Random.h>
#include <functional>
#include <vector>

enum class BoosterType;

struct CellPair {
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

enum class Direction {
	left,
	right,
	top,
	bottom
};

struct Match {
	ComboType comboType;
	PieceId   pieceId;
	int       cellIdx;
	int       cascadeCount;
};

struct Booster {
	BoosterType type;
	int         cellIdx;
};

struct NewPiece {
	int     cellIdx;
	PieceId targetPieceId;
};

struct Match3Event {
	enum class Id {
		swap,
		match,
		removeTile,
		newPiece,
		dropTile,
		newBooster,
		boosterTriggered,
		layerBroken,
	};
	Id id;
	union {
		Match    match;
		CellPair pair;
		NewPiece newPiece;
		Booster  booster;
		int      cellIdx;
	};
};

using Match3Callback = std::function<void(const Match3Event& event)>;

class Board;
class TileSelector;
struct GameConfig;
struct Cell;
struct GameInput;

class Match3 final {
public:
	Match3(Board& board, const GameConfig& gameConfig, TileSelector& tileSelector);
	~Match3();

	void SetCallback(Match3Callback&& cbk);
	void NewBoard(uint32_t seed, const char* boardDef, const int gemIds[], int gemIdCount);
	void Run();
	void Update(const Wind::Input& input);
	int  GetNumUserSwaps() const;
	void AddBooster(BoosterType pieceId, int cellIdx);
	// Boosters
	void HorizontalRocket(int col, int row);
	void VerticalRocket(int col, int row);
	void Bomb(int col, int row, int radius);
	void DeleteAllPieces(int pieceId);

private:
	// States
	void SelectTiles(const Wind::Input& input);
	bool CheckCombos(int h, int v, int t, int b, PieceId pieceId, int cellIdx);
	bool CheckCellCombos(int cellIdx);
	bool CheckMatchesAfterSwap();
	void HitCell(int idx) const;
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

	TileSelector&         mTileSelector;
	Board&                mBoard;
	const GameConfig&     mGameConfig;
	Match3Callback        mCbk;
	int                   mGemIds[8];
	int                   mNumGemIds;
	Wind::Random          mRandomEngine;
	State                 mState;
	CellPair              mUserSwap;
	std::vector<CellPair> mSwaps;
	std::vector<int>      mNewPieces;
	std::vector<int>      mCheckList;
	std::vector<CellPair> mCollapseList;
	std::vector<Booster>  mNewBoosters;
	int                   mNumUserSwaps;
	int                   mCascadeCount;
};
