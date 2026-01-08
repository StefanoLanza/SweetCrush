#include "GameConfig.h"
#include "Constants.h"

GameConfig DefaultGameConfig() {
	GameConfig config;
	config.windowWidth = 768;
	config.windowHeight = 1024;
	config.fullscreen = false;
	config.board.cellWidth = 64.f;
	config.board.cellHeight = 72.f;
	config.board.cellSpacing = 2.f;
	config.board.cellWidthWithSpacing = config.board.cellWidth + config.board.cellSpacing;
	config.board.cellHeightWithSpacing = config.board.cellHeight + config.board.cellSpacing;
	config.board.topLeftCoord.x = (RefWindowWidth - config.board.cellWidthWithSpacing * NumCols - config.board.cellSpacing) * 0.5f;
	config.board.topLeftCoord.y = 300.f;
	config.moveBackPieceSpeed = 0.25f;
	config.swapSpeed = 0.25f;
	config.pieceFallSpeed = 1.f;
	config.pieceFallYCoord = 0.f;
	config.bombExplosionTime = 0.25f;
	config.starMoveTime = 0.5f;
	config.removePieceDuration = 0.25f;
	config.suckPieceDuration = 0.5f;
	config.newPieceDuration = 0.25f;
	config.scoreTextDuration = 1.f;
	config.scoreTextScrollSpeed = 64.f;
	config.brokenIceDuration = 0.25f;
	// UI
	config.ui.swapThreshold = 0.55f;
	config.ui.startDragThreshold = 4.f;
	config.ui.goalStartCoord = { 70.f, 200.f };

	config.settings.musicOn = true;
	config.settings.sfxOn = true;
	config.settings.infoOn = true;
	return config;
}
