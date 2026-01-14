#pragma once

#include <engine/Maths.h>

#include <functional>

namespace Wind {
class Input;
}

struct Cell;

struct TileSelectionEvent {
	enum class Id {
		drag,
		undoDrag,
	};
	Id          id;
	const Cell* cell;
	Wind::Vec2  draggedCoord;
};

using TileSelectionCallback = std::function<void(const TileSelectionEvent& event)>;

class ITileSelector {
public:
	virtual ~ITileSelector() = default;

	virtual int                        GetSelectedTile() const = 0;
	virtual void                       Reset() = 0;
	virtual std::tuple<bool, int, int> SelectTiles(const Wind::Input& input) = 0;
};
