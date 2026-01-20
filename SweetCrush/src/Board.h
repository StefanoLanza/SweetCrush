#pragma once

#include <cstdint>
#include <engine/Maths.h>
#include <engine/Span.h>
#include <vector>

#include "Constants.h"
#include "Effects.h"

using PieceId = uint8_t;

enum class CellCategory {
	empty,
	hole,
	piece,
	obstacle,
	star,
};

struct CellVisual {
	int        bitmapIdx;
	Wind::Vec2 coords;
	float      scale;
	float      bkgAlpha;
	float      rotation;
	uint8_t    priority;
	bool       selected;
	bool       highlighted;
};

struct Cell {
	void*        ud; // user data, e.g. graphics
	Wind::Vec2   coords;
	int          col;
	int          row;
	int          layers;
	EffectType   effectType;
	CellCategory category;
	PieceId      pieceId;
	bool         hasEffect;
};

bool IsSelectable(const Cell& cell);

struct BoardConfig {
	Wind::Vec2 topLeftCoord;
	Wind::Vec2 bottomRightCoord;
	float      cellWidth;
	float      cellHeight;
	float      cellSpacing;
	float      cellWidthWithSpacing;
	float      cellHeightWithSpacing;
};

// Container of cells
class Board final {
public:
	Board(int cols, int rows, const BoardConfig& cfg);

	int                    GetCols() const;
	int                    GetRows() const;
	Wind::Span<Cell>       GetCells(); // TODO Remove
	Wind::Span<const Cell> GetCells() const;
	int                    GetCellCount() const;
	int                    GetCellIndex(int col, int row) const;
	Cell&                  GetCell(int index); // TODO Remove
	const Cell&            GetCell(int index) const;
	Cell&                  GetCell(int col, int row); // TODO Remove
	const Cell&            GetCell(int col, int row) const;
	bool                   IsInside(int col, int row) const;
	int                    TotalLayerCount() const;
	int                    GetCellAtCoords(Wind::Vec2 coords) const;

private:
	std::vector<Cell> mCells;
	BoardConfig       mCfg;
	int               mCols;
	int               mRows;
	// Stats
	int mPieceCount[MaxPieceTypes];
	int mTotalLayerCount;
};
