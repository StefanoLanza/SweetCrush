#pragma once

#include <engine/FwdDecl.h>

class Board;
struct GameConfig;

class GameRenderer final {
public:
	explicit GameRenderer(Wind::Engine& engine);
	~GameRenderer();

	void DrawBoard(const Board& board, int selectedCell, const GameConfig& gameConfig) const;
	void DrawGlow(Wind::Vec2 start, Wind::Vec2 end, float w, float t01) const;

private:
	class Impl;
	std::unique_ptr<Impl> mPimpl;
};
