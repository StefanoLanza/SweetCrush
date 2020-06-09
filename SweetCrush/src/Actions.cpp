#include "Actions.h"
#include "AssetDefs.h"
#include "Board.h"
#include "Config.h"
#include "Constants.h"
#include "GameDrawOrder.h"
#include <cmath>
#include <engine/BitmapRender.h>
#include <engine/Engine.h>
#include <engine/Font.h>
#include <engine/TextRender.h>

using namespace Wind;

ActionFunc MoveTile(Cell& cell, const Vec2& targetCoords, float speed) {
	const Vec2 velocity = Normalize(targetCoords - cell.tileAnim.coords) * speed;
	return [&cell, targetCoords, velocity](float dt, float /*t*/) {
		const Vec2 newCoords = cell.tileAnim.coords + velocity * dt;
		cell.tileAnim.coords = Clamp(newCoords, cell.tileAnim.coords, targetCoords);
		return cell.tileAnim.coords == targetCoords;
	};
}

ActionFunc ReturnTile(Cell& cell, float speed) {
	return MoveTile(cell, cell.coords, speed);
}

ActionFunc ScaleTile(Cell& cell, float startScale, float endScale) {
	return [&cell, startScale, endScale](float /*dt*/, float t) {
		cell.tileAnim.scale = Lerp(endScale, startScale, 1.f - std::pow(t, 2.f));
		return false;
	};
}

ActionFunc DrawMovingSprite(const Cell& cell, const Engine& engine, Vec2 targetPos, int sprite) {
	return [&engine, xy0 = cell.coords, starIconCoord = targetPos, sprite](float /*dt*/, float t) {
		const BitmapRenderer& bitmapRender = engine.GetBitmapRenderer();
		BitmapExtParams       prm;
		prm.scale = 1.f; // + t * 8.f; // TODO curve
		prm.pivot = BitmapPivot::center;
		prm.orientation = t * 3.f;
		prm.drawOrder = static_cast<DrawOrderType>(GameDrawOrder::overlays);
		prm.blending = true;
		Vec2 xy = Lerp(xy0, starIconCoord, t);
		if (sprites[sprite]) {
			bitmapRender.DrawBitmapEx(*sprites[sprite], xy, prm);
		}
		return false;
	};
}

Wind::ActionFunc DrawExplosion(const Cell& cell, const Engine& engine, const GameConfig& gameConfig) {
	Vec2 xy = cell.coords + Vec2 { gameConfig.cellWidth, gameConfig.cellHeight } * 0.5f;
	return [&engine, xy](float /*dt*/, float t) {
		const BitmapRenderer& bitmapRender = engine.GetBitmapRenderer();
		BitmapExtParams       prm;
		prm.scale = 1.f + t * 4.f;
		prm.pivot = BitmapPivot::center;
		prm.drawOrder = static_cast<DrawOrderType>(GameDrawOrder::overlays);
		bitmapRender.DrawBitmapEx(*sprites[sparkleSprite], xy, prm);
		return false;
	};
}

Wind::ActionFunc DrawMatchScore(int score, const Cell& cell, const Engine& engine, const GameConfig& gameConfig, const Font& font) {
	Vec2 xy = cell.coords + Vec2 { gameConfig.cellWidth, gameConfig.cellHeight } * 0.5f;
	return [&engine, &font, xy, score, scrollSpeed = gameConfig.scoreTextScrollSpeed](float /*dt*/, float t) {
		const TextRenderer& textRender = engine.GetTextRenderer();
		char                tmp[64];
		snprintf(tmp, sizeof(tmp), "%d", score);
		float y = xy.y - t * scrollSpeed;
		textRender.Write(font, tmp, Vec2 { xy.x, y }, defaultTextStyle, DrawOrder::UI - 1); // below UI
		return false;
	};
}
