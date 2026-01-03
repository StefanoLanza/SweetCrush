#pragma once

#include <engine/Maths.h>

struct Match3Config {};

struct GameAnimationConfig {};

struct UIConfig {
	float      swapThreshold;      // [0, 1]
	float      startDragThreshold; // pixels
	Wind::Vec2 goalStartCoord;
};

// Configurable settings
struct GameConfig {
	// Graphics
	int   windowWidth;
	int   windowHeight;
	bool  fullscreen;
	float cellWidth;
	float cellHeight;
	float cellSpacing;
	float cellWidthWithSpacing;
	float cellHeightWithSpacing;
	float boardLeft;
	float boardTop;
	int   colorPalette;
	// Animations
	float    moveBackPieceSpeed; // pixels / sec
	float    swapSpeed;
	float    pieceFallSpeed;
	float    pieceFallYCoord;   // pixels
	float    bombExplosionTime; // seconds
	float    starMoveTime;
	float    removePieceDuration; // seconds
	float    scoreTextDuration;
	float    scoreTextScrollSpeed;
	float    brokenIceDuration;
	UIConfig ui;
	// Misc
	bool musicOn;
	bool sfxOn;
	bool infoOn;
	// Match3
	Match3Config match3;
};

GameConfig DefaultGameConfig();
