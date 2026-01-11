#include "GameRenderer.h"
#include "AssetDefs.h"
#include "Board.h"
#include "GameConfig.h"
#include "GameDrawOrder.h"

#include <engine/BitmapRender.h>

using namespace Wind;

void DrawBoardTiles(const Board& board, int selectedCell, const BitmapRenderer& bitmapRender, const GameConfig& gameConfig) {
	const float cellWidth = gameConfig.board.cellWidth;
	const float cellHeight = gameConfig.board.cellHeight;
	const float cellSpacing = gameConfig.board.cellSpacing;
	const float dynScaleFactor = 0.f; // std::cos(mTime * 4.f);

	{
		BitmapExtParams prm;
		prm.width = cellWidth + 2.f * cellSpacing;
		prm.height = cellHeight + 2.f * cellSpacing;
		prm.color = { 255, 255, 255, 140 };
		prm.drawOrder = static_cast<DrawOrder>(GameDrawOrder::backgroundTile);
		prm.blending = true;
		for (const Cell& cell : board.GetCells()) {
			if (cell.category != CellCategory::hole) {
				const BoardTileDef& def = boardTileDefs[0];
				bitmapRender.DrawBitmapEx(*sprites[def.sprite], cell.coords - Vec2 { cellSpacing, cellSpacing }, prm);
			}
		}
	}

	// Draw pieces, obstacles and boosters
	for (const Cell& cell : board.GetCells()) {
		Vec2            pos = cell.pieceGraphics.coords + Vec2 { cellWidth, cellHeight } * 0.5f;
		BitmapExtParams prm;
		prm.width = cellWidth;
		prm.height = cellHeight;
		prm.pivot = BitmapPivot::center;
		prm.blending = true;
		if (cell.pieceGraphics.bitmapIdx != -1) {
			prm.orientation = 0.f; // mTime * cell.pieceGraphics.rotation;
			prm.scale.x = cell.pieceGraphics.scale;
			prm.scale.y = cell.pieceGraphics.scale;
			prm.drawOrder = static_cast<DrawOrder>(GameDrawOrder::boardTile);
			bitmapRender.DrawBitmapEx(*sprites[cell.pieceGraphics.bitmapIdx], pos, prm);
			if (cell.layers > 0) {
				prm.drawOrder = static_cast<DrawOrder>(GameDrawOrder::ice);
				prm.orientation = 0.0f;
				prm.scale.x = 1.f;
				prm.scale.y = 1.f;
				bitmapRender.DrawBitmapEx(*sprites[iceSprite], pos, prm);
			}
		}
		if (cell.hasBooster) {
			// TODO REmove, draw different bitmap
			prm.scale.x = 0.5f + 0.1f * dynScaleFactor;
			prm.scale.y = 0.5f + 0.1f * dynScaleFactor;
			prm.drawOrder = static_cast<DrawOrder>(GameDrawOrder::ice);
			prm.orientation = 0.f;
			pos.x += cellWidth * 0.25f;
			pos.y += cellHeight * 0.25f;
			bitmapRender.DrawBitmapEx(*sprites[boosterDefs[(int)cell.boosterType].sprite], pos, prm);
		}
	}
	// Highlight selected cell
	if (selectedCell >= 0) {
		const Cell&     cell = board.GetCell(selectedCell);
		BitmapExtParams prm;
		prm.width = cellWidth + 2.f * cellSpacing;
		prm.height = cellHeight + 2.f * cellSpacing;
		prm.pivot = BitmapPivot::topLeft;
		prm.drawOrder = static_cast<DrawOrder>(GameDrawOrder::boardTile);
		prm.blending = true;
		bitmapRender.DrawBitmapEx(*sprites[selectionSprite], cell.pieceGraphics.coords - Vec2 { cellSpacing, cellSpacing }, prm);
	}
}