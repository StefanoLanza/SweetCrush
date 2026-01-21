#pragma once

#include <engine/FwdDecl.h>

class Board;
struct AppConfig;

class GameRenderer final {
public:
	explicit GameRenderer(Wind::Engine& engine);
	~GameRenderer();

	void DrawBoard(const Board& board, int selectedCell, const AppConfig& gameConfig, float time) const;
	void DrawLaser(Wind::Vec2 start, Wind::Vec2 end, float w) const;
	void DrawBlast(Wind::Vec2 center, float radius) const;
	void DrawIcon(uint32_t iconIdx, Wind::Vec2 coords, float rotation, const Wind::Color& color, unsigned drawOrder) const;

private:
	class Impl;
	std::unique_ptr<Impl> mPimpl;
};
