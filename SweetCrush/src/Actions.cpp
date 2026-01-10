#include "Actions.h"
#include "AssetDefs.h"
#include "Board.h"
#include "Constants.h"
#include "GameConfig.h"
#include "GameDrawOrder.h"

#include <engine/BitmapRender.h>
#include <engine/Easings.h>
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
		cell.pieceGraphics.coords = LerpEase(initialCoords, targetCoords, t01, EaseInOutCirc);
		return false;
	};
}

ActionFunc MovePieceFromTo(Cell& cell, const Wind::Vec2& startCoords, const Wind::Vec2& endCoords) {
	return [&cell, startCoords, endCoords](float dt, float t01) {
		cell.pieceGraphics.coords = LerpEase(startCoords, endCoords, t01, EaseInQuad);
		return false;
	};
}

ActionFunc FallPieceFromTo(Cell& cell, float startCoord, float endCoord) {
	cell.pieceGraphics.coords.x = cell.coords.x;
	return [&cell, startCoord, endCoord](float dt, float t01) {
		cell.pieceGraphics.coords.y = LerpEase(startCoord, endCoord, t01, EaseOutBounce);
		return false;
	};
}

ActionFunc MovePieceTo(Cell& cell, const Wind::Vec2& targetCoords, float speed) {
	const Vec2 velocity = Normalize(targetCoords - cell.pieceGraphics.coords) * speed;
	return [&cell, targetCoords, velocity](float dt, float /*t01*/) {
		cell.pieceGraphics.coords = Clamp(cell.pieceGraphics.coords + velocity * dt, cell.pieceGraphics.coords, targetCoords);
		return cell.pieceGraphics.coords == targetCoords;
	};
}

ActionFunc ScaleCellPiece(Cell& cell, float startScale, float endScale) {
	return [&cell, startScale, endScale](float /*dt*/, float t01) {
		cell.pieceGraphics.scale = LerpEase(startScale, endScale, t01, EaseInQuad);
		return false;
	};
}

ActionFunc DrawMovingSprite(const Cell& cell, const BitmapRenderer& bitmapRenderer, Vec2 targetPos, int sprite) {
	return [&bitmapRenderer, xy0 = cell.coords, starIconCoord = targetPos, sprite](float /*dt*/, float t01) {
		BitmapExtParams prm;
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
		prm.scale.x = 1.f + t * 4.f;
		prm.scale.y = prm.scale.x;
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
		prm.scale.x = 1.f + t01 * 0.5f;
		prm.scale.y = prm.scale.x;
		prm.pivot = BitmapPivot::center;
		prm.drawOrder = static_cast<DrawOrderType>(GameDrawOrder::ice);
		prm.blending = true;
		prm.color.a = LerpEase(255.f, 0.f, t01, EaseInCubic);
		bitmapRenderer.DrawBitmapEx(*sprites[brokenIceSprite], xy, prm);
		return false;
	};
}

ActionFunc DrawGlow(Vec2 xy, bool horizontal, const Wind::BitmapRenderer& bitmapRenderer) {
	return [&bitmapRenderer, xy, horizontal](float /*dt*/, float t01) {
		BitmapExtParams prm;
		prm.scale.x = 0.25f + t01 * 4.f;
		prm.scale.y = 1.f;
		prm.pivot = BitmapPivot::center;
		prm.drawOrder = static_cast<DrawOrderType>(GameDrawOrder::overlays);
		prm.orientation = horizontal ? 0.f : half_pi;
		prm.blending = true;
		prm.color.r = 512.f;
		prm.color.g = 512.f;
		prm.color.b = 512.f;
		prm.color.a = LerpEase(0.f, 255.f, t01, EaseInQuint);
		bitmapRenderer.DrawBitmapEx(*sprites[glowSprite], xy, prm);
		return false;
	};
}
