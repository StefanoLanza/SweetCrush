#pragma once

#include <engine/ActionMgr.h>
#include <engine/Math.h>
#include <engine/FwdDecl.h>

struct Cell;
struct GameConfig;

// Predefined actions
Wind::ActionFunc MoveTile(Cell& cell, const Wind::Vec2& targetCoords, float speed);
Wind::ActionFunc ReturnTile(Cell& cell, float speed);
Wind::ActionFunc ScaleTile(Cell& cell, float startScale, float endScale);
Wind::ActionFunc DrawMovingStar(const Cell& cell, const Wind::Engine& engine, Wind::Vec2 targetPos);
Wind::ActionFunc DrawExplosion(const Cell& cell, const Wind::Engine& engine, const GameConfig& gameConfig);
Wind::ActionFunc DrawMatchScore(int score, const Cell& cell, const Wind::Engine& engine, const GameConfig& gameConfig, const Wind::Font& font);
