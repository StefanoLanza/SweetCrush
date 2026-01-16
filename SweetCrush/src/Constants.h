#pragma once

constexpr int NumCols = 9;
constexpr int NumRows = 9;
constexpr int RefWindowWidth = 640;
constexpr int RefWindowHeight = 1136;
constexpr int MaxPieceTypes = 8;
constexpr float TileWidth = 64.f;
constexpr float TileHeight = 72.f;

using CellID = unsigned char;

// CellId equal to ASCII code abcd etc. up to (MaxPieceTypes - 1) corresponds to pieces
// Capital letter indicates a layer (ice)
constexpr CellID emptyChar = '_';
constexpr CellID holeChar = ' ';
constexpr CellID obstacleChar = 'O';
constexpr CellID starChar = 's';
// TODO add more
