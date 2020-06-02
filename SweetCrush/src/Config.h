#pragma once

#include <engine/Math.h>

// Configurable settings
struct GameConfig {
	// Graphics
	int   windowWidth;
	int   windowHeight;
	float cellWidth;
	float cellHeight;
	float cellSpacing;
	float cellWidthWithSpacing;
	float cellHeightWithSpacing;
	float boardLeft;
	float boardTop;
	int   colorPalette;
	// Animations
	float tileMoveBackSpeed; // pixels / sec
	float tileSwapSpeed;
	float tileFallSpeed;
	float tileFallYCoord;    // pixels
	float bombExplosionTime; // seconds
	float starMoveTime;
	float tileScaleDuration; // seconds
	float scoreTextDuration;
	float scoreTextScrollSpeed;
	// UI
	float      swapThreshold;      // [0, 1]
	float      startDragThreshold; // pixels
	Wind::Vec2 targetGemCoord;
	// Misc
	bool audioOn;
};

GameConfig DefaultGameConfig();
