#pragma once

#include "ActionMgr.h"
#include <engine/FwdDecl.h>
#include <engine/Maths.h>

struct Cell;
struct GameConfig;

// Predefined actions
ActionFunc MoveTile(Cell& cell, const Wind::Vec2& targetCoords, float speed);
ActionFunc ReturnTile(Cell& cell, float speed);
ActionFunc ScaleTile(Cell& cell, float startScale, float endScale);
ActionFunc DrawMovingSprite(const Cell& cell, const Wind::Engine& engine, Wind::Vec2 targetPos, int sprite);
ActionFunc DrawExplosion(const Cell& cell, const Wind::Engine& engine, const GameConfig& gameConfig);
ActionFunc DrawMatchScore(int score, const Cell& cell, const Wind::Engine& engine, const GameConfig& gameConfig, const Wind::Font& font);
