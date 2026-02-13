#pragma once

#include "GameSettings.h"
#include "Board.h"
#include <engine/Maths.h>

struct Match3Config {};

struct GameAnimationConfig {};

struct UIConfig {
	float      swapThreshold;      // [0, 1]
	float      startDragThreshold; // pixels
};


// Configurable settings
struct AppConfig {
	// Graphics
	int         windowWidth;
	int         windowHeight;
	bool        fullscreen;
	BoardConfig board;
	// Animations
	float        moveBackPieceDuration; // sec
	float        swapSpeed;
	float        pieceFallDuration; // pixels / sec
	float        pieceFallSpeed;
	float        pieceFallYCoord;   // pixels
	float        bombExplosionTime; // seconds
	float        glowTrailTime;
	float        starMoveTime;
	float        removePieceDuration; // seconds
	float        suckPieceDuration;
	float        newPieceDuration; // seconds
	float        scoreTextDuration;
	float        scoreTextScrollSpeed;
	float        brokenIceDuration;
	UIConfig     ui;
	GameSettings settings;
	Match3Config match3;
};

AppConfig DefaultGameConfig();
