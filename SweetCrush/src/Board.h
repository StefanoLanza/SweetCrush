#pragma once

#include <cstdint>
#include <engine/Maths.h>
#include <engine/Span.h>
#include <vector>

#include "Boosters.h"

using PieceId = uint8_t;

enum class CellCategory {
	empty,
	hole,
	piece,
	obstacle,
};

struct PieceAnim {
	Wind::Vec2 coords;
	int        spriteIdx;
	float      scale;
	float      scaleDev;
	float      rotation;
};

struct Cell {
	Wind::Vec2   coords;
	int          col;
	int          row;
	int          layers;
	PieceId      pieceId;
	BoosterType  boosterType;
	PieceAnim    pieceAnim;
	CellCategory category;
	bool         hasBooster;
	uint8_t      backgroundTileIdx;
};

bool IsEmpty(const Cell& cell);
bool IsHole(const Cell& cell);
bool IsPiece(const Cell& cell);
bool IsObstacle(const Cell& cell);
bool IsSelectable(const Cell& cell);

// Container of cells
class Board final {
public:
	Board(int cols, int rows);

	int                    GetCols() const;
	int                    GetRows() const;
	Wind::Span<Cell>       GetCells();
	Wind::Span<const Cell> GetCells() const;
	int                    GetCellCount() const;
	int                    GetCellIndex(int col, int row) const;
	Cell&                  GetCell(int index);
	const Cell&            GetCell(int index) const;
	Cell&                  GetCell(int col, int row);
	const Cell&            GetCell(int col, int row) const;
	bool                   IsInside(int col, int row) const;

private:
	std::vector<Cell> mCells;
	int               mCols;
	int               mRows;
};
