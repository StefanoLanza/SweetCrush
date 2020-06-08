#pragma once

struct MatchStats {
	int  score;
	int  level;
	int  targetGemCount[3];
	bool gameComplete;
	bool levelComplete;
	bool restartLevel;
};
