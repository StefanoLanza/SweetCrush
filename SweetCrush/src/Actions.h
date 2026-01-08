#pragma once

#include "ActionMgr.h"
#include <engine/FwdDecl.h>
#include <engine/Maths.h>

struct Cell;
struct GameConfig;

// Predefined actions
ActionFunc MoveBackPiece(Cell& cell);
ActionFunc MovePieceTo(Cell& cell, const Wind::Vec2& targetCoords);
ActionFunc ScaleCellPiece(Cell& cell, float startScale, float endScale);
ActionFunc DrawMovingSprite(const Cell& cell, const Wind::BitmapRenderer& bitmapRenderer, Wind::Vec2 targetPos, int sprite);
ActionFunc DrawExplosion(const Cell& cell, const Wind::BitmapRenderer& bitmapRenderer, const GameConfig& gameConfig);
ActionFunc DrawMatchScore(int score, const Cell& cell, const Wind::TextRenderer& textRenderer, const GameConfig& gameConfig, const Wind::Font& font);
ActionFunc DrawBrokenIce(const Cell& cell, const Wind::BitmapRenderer& bitmapRenderer, const GameConfig& gameConfig);
