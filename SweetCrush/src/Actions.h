#pragma once

#include <engine/ActionMgr.h>
#include <engine/FwdDecl.h>
#include <engine/Maths.h>

struct Cell;
struct AppConfig;
struct CellVisual;
class GameRenderer;

// Predefined actions
Wind::ActionFunc MovePieceTo(CellVisual& cell, const Wind::Vec2& targetCoords);
Wind::ActionFunc MovePieceFromTo(CellVisual& cell, const Wind::Vec2& startCoords, const Wind::Vec2& endCoords);
Wind::ActionFunc FallPieceFromTo(CellVisual& cell, float xCoord, float startYCoord, float endYCoord);
Wind::ActionFunc MovePieceTo(CellVisual& cell, const Wind::Vec2& targetCoords, float speed);
Wind::ActionFunc ScalePiece(CellVisual& cell, float startScale, float endScale);
Wind::ActionFunc FadeInAlpha(CellVisual& visual);
Wind::ActionFunc DrawMovingSprite(const Cell& cell, const Wind::BitmapRenderer& bitmapRenderer, Wind::Vec2 targetPos, int sprite);
Wind::ActionFunc DrawMatchScore(int score, const Cell& cell, const Wind::TextRenderer& textRenderer, const AppConfig& gameConfig, const Wind::Font& font);
Wind::ActionFunc DrawBrokenIce(const Cell& cell, const Wind::BitmapRenderer& bitmapRenderer, const AppConfig& gameConfig);
Wind::ActionFunc DrawLaser(Wind::Vec2 startCoords, Wind::Vec2 endCoords, const GameRenderer& bitmapRenderer);
Wind::ActionFunc DrawBlast(Wind::Vec2 center, float startRadius, float endRadius, const GameRenderer& gameRenderer);
