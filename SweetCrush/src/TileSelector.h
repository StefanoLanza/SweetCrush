#pragma once

#include "ITileSelector.h"
#include <engine/FwdDecl.h>
#include <engine/Maths.h>

class Board;
struct AppConfig;
struct Cell;

class TileSelector final : public ITileSelector {
public:
	TileSelector(const Board& board, const AppConfig& gameConfig);

	void                       SetCallback(TileSelectionCallback&& cbk);
	int                        GetSelectedTile() const override;
	void                       Reset() override;
	std::tuple<bool, int, int> SelectTiles(const Wind::Input& input) override;

private:
	void SelectFirstCell(int idx);
	void StartDrag(float mouseX, float mouseY);
	bool DragTileX(const Cell& cell, float deltaX, float threshold) const;
	bool DragTileY(const Cell& cell, float deltaY, float threshold) const;
	void UndoDrag();

private:
	enum class State;
	enum class DragDirection;

	const Board&          mBoard;
	const AppConfig&     mGameConfig;
	TileSelectionCallback mCbk;
	State                 mState;
	int                   mFirstCellIdx;
	int                   mSelectedCellIdx;
	DragDirection         mDragDirection;
	Wind::Vec2            mDragMouseCoords;
};
