#pragma once

constexpr int NumCols = 9;
constexpr int NumRows = 9;
constexpr int RefWindowWidth = 640;
constexpr int RefWindowHeight = 1136;
constexpr int MaxPieceTypes = 8;

using CellID = unsigned char;

// CellIds equal to ASCII code [0, MaxPieceTypes - 1] correspond to pieces
// The actual gem is gemIds[cellId]
constexpr CellID emptyCell = '_';
constexpr CellID holeCell = ' ';
constexpr CellID obstacleCell = 'O';
// TODO add more
