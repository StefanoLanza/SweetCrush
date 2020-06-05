#pragma once

#include "Board.h"
#include <engine/FwdDecl.h>
#include <functional>
#include <random>
#include <vector>

struct CellPair {
	int first;
	int second;
};

enum class ComboType { C3, C4, C5, T3, T4, T5, L, X };

enum class Direction { left, right, top, bottom };

struct Match {
	ComboType comboType;
	TileId    tileId;
	int       cellIdx;
	int       cascadeCount;
};

struct Booster {
	BoosterType type;
	int         cellIdx;
};

struct NewGem {
	int    cellIdx;
	TileId targetGemId;
};

struct Match3Event {
	enum class Id {
		swap,
		match,
		removeTile,
		newGem,
		dropTile,
		newBooster,
		boosterTriggered,
	};
	Id id;
	union {
		Match    match;
		CellPair pair;
		NewGem   newGem;
		Booster  booster;
		int      cellIdx;
	};
};

using Match3Callback = std::function<void(const Match3Event& event)>;

class Board;
class TileSelector;
struct GameConfig;

class Match3 {
public:
	Match3(Board& board, const GameConfig& gameConfig, TileSelector& tileSelector);
	~Match3();

	void SetCallback(Match3Callback&& cbk);
	void NewBoard(uint32_t seed, const int gemIds[], int gemIdCount);
	void Run();
	void Update();
	int  GetNumUserSwaps() const;
	void AddBooster(BoosterType tileId, int cellIdx);
	// Boosters
	void HorizontalRocket(int col, int row);
	void VerticalRocket(int col, int row);
	void MiniBomb(int col, int row);
	void Bomb(int col, int row);

private:
	// States
	void SelectTiles();
	bool CheckCombos(int h, int v, int t, int b, TileId tileId, int cellIdx);
	bool CheckCellCombos(int cellIdx);
	bool CheckMatchesAfterSwap();
	void RemoveTile(int idx) const;
	void InsertBoosters();
	void CollapseColumns();
	void GenerateNewGems();
	bool CheckMatches();

	void TrySwap(int first, int second);
	void SwapSelectedTiles(int firstTile, int secondTile);
	void CollapseColumn(int col);
	void KillMatches(const Cell& cell, int dcol, int drow);
	void TriggerBooster(int cellIdx);
	void DeleteAllTilesSameType(int tileId);

private:
	enum class State;
	using RandomEngine = std::default_random_engine;

	TileSelector&         mTileSelector;
	Board&                mBoard;
	const GameConfig&     mGameConfig;
	Match3Callback        mCbk;
	int                   mGemIds[8];
	int                   mNumGemIds;
	RandomEngine          mRandomEngine;
	State                 mState;
	CellPair              mUserSwap;
	std::vector<CellPair> mSwaps;
	std::vector<int>      mNewGems;
	std::vector<int>      mCheckList;
	std::vector<CellPair> mCollapseList;
	std::vector<Booster>  mBoosters;
	int                   mNumUserSwaps;
	int                   mCascadeCount;
};
