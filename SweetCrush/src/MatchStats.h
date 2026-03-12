#pragma once

struct MatchStats {
	int score;
	int moves;
	int levelIndex;
	int targetPieceCount[3];
	int layerCount;
};
