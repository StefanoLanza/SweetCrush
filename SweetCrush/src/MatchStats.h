#pragma once

struct MatchStats {
	int  score;
	int  level;
	int  targetPieceCount[3];
	int  layerCount;
	bool gameComplete;
	bool levelComplete;
	bool restartLevel;
};
