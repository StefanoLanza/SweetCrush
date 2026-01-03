#pragma once

#include "ITileSelector.h"
#include <engine/FwdDecl.h>
#include <engine/Maths.h>

class Board;
struct GameConfig;
struct CellPairEvent;
struct Cell;

class TileSelector final : public ITileSelector {
public:
	TileSelector(Board& boardDef, const GameConfig& gameConfig);

	void                       AddCallback(TileSelectionCallback&& cbk);
	int                        GetSelectedCell() const override;
	void                       Reset() override;
	std::tuple<bool, int, int> SelectTiles(const Wind::Input& input) override;

private:
	void SelectFirstCell(int idx);
	void StartDrag(float mouseX, float mouseY);
	bool DragTileX(Cell& cell, float deltaX, float threshold) const;
	bool DragTileY(Cell& cell, float deltaY, float threshold) const;
	void UndoDrag();

private:
	enum class State;
	enum class DragDirection;

	Board&                mBoard;
	const GameConfig&     mGameConfig;
	TileSelectionCallback mCbk;
	State                 mState;
	int                   mFirstCellIdx;
	int                   mSelectedCellIdx;
	DragDirection         mDragDirection;
	Wind::Vec2            mDragMouseCoords;
};
