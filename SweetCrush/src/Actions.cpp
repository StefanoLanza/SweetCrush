#include "Actions.h"
#include "AssetDefs.h"
#include "Board.h"
#include "Constants.h"
#include "AppConfig.h"
#include "GameDrawOrder.h"
#include "GameRenderer.h"

#include <engine/BitmapRender.h>
#include <engine/Easings.h>
#include <engine/TextRender.h>

#include <cassert>
#include <cmath>
#include <cstdio>

using namespace Wind;

ActionFunc MovePieceTo(CellVisual& cell, const Vec2& targetCoords) {
	return [&cell, initialCoords = cell.coords, targetCoords](float dt, float t01) {
		cell.coords = LerpEase(initialCoords, targetCoords, t01, EaseInOutCirc);
		return false;
	};
}

ActionFunc MovePieceFromTo(CellVisual& cell, const Wind::Vec2& startCoords, const Wind::Vec2& endCoords) {
	return [&cell, startCoords, endCoords](float dt, float t01) {
		cell.coords = LerpEase(startCoords, endCoords, t01, EaseInQuad);
		return false;
	};
}

ActionFunc FallPieceFromTo(CellVisual& cell, float xCoord, float startYCoord, float endYCoord) {
	cell.coords.x = xCoord;
	return [&cell, startYCoord, endYCoord](float dt, float t01) {
		cell.coords.y = LerpEase(startYCoord, endYCoord, t01, EaseOutBounce);
		return false;
	};
}

ActionFunc MovePieceTo(CellVisual& visual, const Wind::Vec2& targetCoords, float speed) {
	const Vec2 velocity = Normalize(targetCoords - visual.coords) * speed;
	return [&visual, targetCoords, velocity](float dt, float /*t01*/) {
		visual.coords = Clamp(visual.coords + velocity * dt, visual.coords, targetCoords);
		return visual.coords == targetCoords;
	};
}

ActionFunc ScalePiece(CellVisual& visual, float startScale, float endScale) {
	return [&visual, startScale, endScale](float /*dt*/, float t01) {
		visual.scale = LerpEase(startScale, endScale, t01, EaseInQuad);
		return false;
	};
}

ActionFunc FadeInAlpha(CellVisual& visual) {
	return [&visual](float /*dt*/, float t01) {
		visual.bkgAlpha = LerpEase(0.f, 1.f, t01, EaseInQuad);
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

ActionFunc DrawExplosion(const Cell& cell, const BitmapRenderer& bitmapRenderer, const AppConfig& gameConfig) {
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

ActionFunc DrawMatchScore(int score, const Cell& cell, const TextRenderer& textRenderer, const AppConfig& gameConfig, const Font& font) {
	Vec2 xy = cell.coords + Vec2 { gameConfig.board.cellWidth, gameConfig.board.cellHeight } * 0.5f;
	return [&textRenderer, &font, xy, score, scrollSpeed = gameConfig.scoreTextScrollSpeed](float /*dt*/, float t) {
		char tmp[64];
		snprintf(tmp, sizeof(tmp), "%d", score);
		float y = xy.y - t * scrollSpeed;
		textRenderer.Write(font, tmp, Vec2 { xy.x, y }, defaultTextStyle, DrawOrder::UI - 1); // below UI
		return false;
	};
}

ActionFunc DrawBrokenIce(const Cell& cell, const BitmapRenderer& bitmapRenderer, const AppConfig& gameConfig) {
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

ActionFunc DrawLaser(Vec2 startCoords, Vec2 endCoords, const GameRenderer& gameRenderer) {
	return [&gameRenderer, startCoords, endCoords](float /*dt*/, float t01) {
		gameRenderer.DrawLaser(startCoords, endCoords, 64.f, EaseInQuint(t01));
		return false;
	};
}
