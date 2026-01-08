#include "Actions.h"
#include "AssetDefs.h"
#include "Board.h"
#include "Constants.h"
#include "GameConfig.h"
#include "GameDrawOrder.h"
#include <engine/BitmapRender.h>
#include <engine/TextRender.h>

#include <cassert>
#include <cmath>
#include <cstdio>

using namespace Wind;

ActionFunc MoveBackPiece(Cell& cell) {
	return MovePieceTo(cell, cell.coords);
}

ActionFunc MovePieceTo(Cell& cell, const Vec2& targetCoords) {
	return [&cell, initialCoords = cell.pieceGraphics.coords, targetCoords](float dt, float t01) {
		t01 = t01 * t01;
		cell.pieceGraphics.coords = Lerp(initialCoords, targetCoords, t01);
		return false;
	};
}

ActionFunc ScaleCellPiece(Cell& cell, float startScale, float endScale) {
	return [&cell, startScale, endScale](float /*dt*/, float t01) {
		cell.pieceGraphics.scale = Lerp(endScale, startScale, 1.f - t01 * t01);
		return false;
	};
}

ActionFunc DrawMovingSprite(const Cell& cell, const BitmapRenderer& bitmapRenderer, Vec2 targetPos, int sprite) {
	return [&bitmapRenderer, xy0 = cell.coords, starIconCoord = targetPos, sprite](float /*dt*/, float t01) {
		BitmapExtParams prm;
		prm.scale = 1.f; // + t * 8.f; // TODO curve
		prm.pivot = BitmapPivot::center;
		prm.orientation = t01 * 3.f;
		prm.drawOrder = static_cast<DrawOrderType>(GameDrawOrder::overlays);
		prm.blending = true;
		Vec2 xy = Lerp(xy0, starIconCoord, t01);
		bitmapRenderer.DrawBitmapEx(*sprites[sprite], xy, prm);
		return false;
	};
}

ActionFunc DrawExplosion(const Cell& cell, const BitmapRenderer& bitmapRenderer, const GameConfig& gameConfig) {
	Vec2 xy = cell.coords + Vec2 { gameConfig.board.cellWidth, gameConfig.board.cellHeight } * 0.5f;
	return [&bitmapRenderer, xy](float /*dt*/, float t) {
		BitmapExtParams prm;
		prm.scale = 1.f + t * 4.f;
		prm.pivot = BitmapPivot::center;
		prm.drawOrder = static_cast<DrawOrderType>(GameDrawOrder::overlays);
		prm.blending = true;
		bitmapRenderer.DrawBitmapEx(*sprites[sparkleSprite], xy, prm);
		return false;
	};
}

ActionFunc DrawMatchScore(int score, const Cell& cell, const TextRenderer& textRenderer, const GameConfig& gameConfig, const Font& font) {
	Vec2 xy = cell.coords + Vec2 { gameConfig.board.cellWidth, gameConfig.board.cellHeight } * 0.5f;
	return [&textRenderer, &font, xy, score, scrollSpeed = gameConfig.scoreTextScrollSpeed](float /*dt*/, float t) {
		char tmp[64];
		snprintf(tmp, sizeof(tmp), "%d", score);
		float y = xy.y - t * scrollSpeed;
		textRenderer.Write(font, tmp, Vec2 { xy.x, y }, defaultTextStyle, DrawOrder::UI - 1); // below UI
		return false;
	};
}

ActionFunc DrawBrokenIce(const Cell& cell, const BitmapRenderer& bitmapRenderer, const GameConfig& gameConfig) {
	Vec2 xy = cell.coords + Vec2 { gameConfig.board.cellWidth, gameConfig.board.cellHeight } * 0.5f;
	return [&bitmapRenderer, xy](float /*dt*/, float t01) {
		BitmapExtParams prm;
		prm.scale = 1.f + t01 * 0.5f;
		prm.pivot = BitmapPivot::center;
		prm.drawOrder = static_cast<DrawOrderType>(GameDrawOrder::ice);
		prm.blending = true;
		prm.color.a = 255.f * (1.f - t01 * t01 * t01 * t01); // ease-in
		bitmapRenderer.DrawBitmapEx(*sprites[brokenIceSprite], xy, prm);
		return false;
	};
}
