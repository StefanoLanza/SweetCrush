#pragma once

constexpr int NumCols = 8;
constexpr int NumRows = 8;
constexpr int RefWindowWidth = 768;
constexpr int RefWindowHeight = 1024;
constexpr int MaxPieceTypes = 8;

using CellID = unsigned char;

// CellIds equal to ASCII code [0, MaxGemTypes - 1] correspond to gems
// The actual gem is gemIds[cellId]
constexpr CellID emptyCell = 'E';
constexpr CellID holeCell = 'H';
constexpr CellID obstacleCell = 'O';
// TODO add more
