#pragma once

#include <functional>

namespace Wind {
	class Input;
}
struct TileSelectionEvent {
	enum class Id { select, deselect, drag, undoDrag };
	Id  id;
	int cellIdx;
};

using TileSelectionCallback = std::function<void(const TileSelectionEvent& event)>;

class ITileSelector {
public:
	virtual ~ITileSelector() = default;

	virtual int                        GetSelectedCell() const = 0;
	virtual void                       Reset() = 0;
	virtual std::tuple<bool, int, int> SelectTiles(const Wind::Input& input) = 0;
};
