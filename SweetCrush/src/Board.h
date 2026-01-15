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
	float      rotation;
	uint8_t    priority;
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

// Container of cells
class Board final {
public:
	Board(int cols, int rows);

	int                    GetCols() const;
	int                    GetRows() const;
	Wind::Span<Cell>       GetCells(); // TODO Remove
	Wind::Span<const Cell> GetCells() const;
	int                    GetCellCount() const;
	int                    GetCellIndex(int col, int row) const;
	Cell&                  GetCell(int index); // TODO Remove
	//void                   ReplaceCell(int index, const Cell& cell, void* ud);
	//void                   ReplaceCell(int col, int row, const Cell& cell, void* ud);
	const Cell&            GetCell(int index) const;
	Cell&                  GetCell(int col, int row); // TODO Remove
	const Cell&            GetCell(int col, int row) const;
	bool                   IsInside(int col, int row) const;
	int                    TotalLayerCount() const;

private:
	std::vector<Cell> mCells;
	int               mCols;
	int               mRows;
	// Stats
	int mPieceCount[MaxPieceTypes];
	int mTotalLayerCount;
};
