#include "Config.h"
#include "Constants.h"

GameConfig DefaultGameConfig() {
	GameConfig config;
	config.windowWidth = 576;
	config.windowHeight = 768;
	config.cellWidth = 64.f;
	config.cellHeight = 72.f;
	config.cellSpacing = 2.f;
	config.cellWidthWithSpacing = config.cellWidth + config.cellSpacing;
	config.cellHeightWithSpacing = config.cellHeight + config.cellSpacing;
	config.boardLeft = (RefWindowWidth - config.cellWidthWithSpacing * NumCols - config.cellSpacing) * 0.5f;
	config.boardTop = 300.f;
	config.colorPalette = 0;
	config.tileMoveBackSpeed = 256.f;
	config.tileSwapSpeed = 256.f;
	config.tileFallSpeed = 1024.f;
	config.tileFallYCoord = 0.f;
	config.bombExplosionTime = 0.25f;
	config.starMoveTime = 0.5f;
	config.tileScaleDuration = 0.25f;
	config.scoreTextDuration = 1.f;
	config.scoreTextScrollSpeed = 32.f;
	// UI
	config.swapThreshold = 0.55f;
	config.startDragThreshold = 4.f;
	config.targetGemCoord = { 70.f, 200.f };

	config.audioOn = true;
	return config;
}
