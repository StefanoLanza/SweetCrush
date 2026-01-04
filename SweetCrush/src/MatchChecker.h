#pragma once

class Board;
struct Cell;

enum class CheckDirection {
	left,
	right,
	top,
	bottom
};

bool CheckMatch(const Cell& a, const Cell& b);
int CountMatches(const Cell& cell, const Board& board, CheckDirection dir);
