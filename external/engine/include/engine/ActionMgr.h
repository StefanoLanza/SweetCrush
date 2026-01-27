#pragma once

#include "Id.h"

#include <cstdint>
#include <functional>
#include <vector>

namespace Wind {

// Returns true if complete
using ActionFunc = std::function<bool(float dt, float t)>;

using ActionId = Id<ActionFunc, uint16_t>;

struct ActionInfo {
	float duration = -1;
	float delay = 0;
};

class ActionMgr final {
public:
	ActionMgr();
	~ActionMgr();

	ActionId AddAction(ActionFunc&& func, float delay = 0.f);
	ActionId AddTimedAction(ActionFunc&& func, float duration, float delay = 0);
	ActionId AddContinuation(ActionId parentId, ActionFunc&& func);
	void     Execute(float dt);
	bool     IsFinished(ActionId actionId) const;
	void     Clear();
	bool     AnyRunning() const;

private:
	struct Action;
	std::vector<Action> mActions;
};

} // namespace Wind