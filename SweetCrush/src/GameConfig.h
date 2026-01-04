#pragma once

#include "GameSettings.h"
#include <engine/Maths.h>

struct Match3Config {};

struct GameAnimationConfig {};

struct UIConfig {
	float      swapThreshold;      // [0, 1]
	float      startDragThreshold; // pixels
	Wind::Vec2 goalStartCoord;
};

struct BoardConfig {
	Wind::Vec2 topLeftCoord;
	float      cellWidth;
	float      cellHeight;
	float      cellSpacing;
	float      cellWidthWithSpacing;
	float      cellHeightWithSpacing;
};

// Configurable settings
struct GameConfig {
	// Graphics
	int         windowWidth;
	int         windowHeight;
	bool        fullscreen;
	BoardConfig board;
	// Animations
	float        moveBackPieceSpeed; // pixels / sec
	float        swapSpeed;
	float        pieceFallSpeed;
	float        pieceFallYCoord;   // pixels
	float        bombExplosionTime; // seconds
	float        starMoveTime;
	float        removePieceDuration; // seconds
	float        newPieceDuration; // seconds
	float        scoreTextDuration;
	float        scoreTextScrollSpeed;
	float        brokenIceDuration;
	UIConfig     ui;
	GameSettings settings;
	Match3Config match3;
};

GameConfig DefaultGameConfig();
