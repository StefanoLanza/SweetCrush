#pragma once

#include <engine/FwdDecl.h>

class Board;
struct GameConfig;

class GameRenderer final {
public:
	explicit GameRenderer(Wind::Engine& engine);
	~GameRenderer();

	void DrawBoard(const Board& board, int selectedCell, const GameConfig& gameConfig) const;

private:
	class Impl;
	std::unique_ptr<Impl> mPimpl;
};
