#pragma once

#include "Id.h"

#include <cstdint>
#include <functional>
#include <vector>

namespace Wind {

struct ActionTag;
using ActionId = Id<ActionTag, uint16_t>;

// Returns true if complete
using ActionFunc = std::function<bool(float dt, float t)>;

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
	ActionId AddChild(ActionId parentId, ActionFunc&& func);
	ActionId AddContinuation(ActionId parentId, ActionFunc&& func, float duration);
	void     Execute(float dt);
	bool     IsFinished(ActionId actionId) const;
	void     Clear();
	bool     AnyRunning() const;

private:
	ActionId NewAction(ActionFunc&& func, float duration, float delay);
	void     PushAction(uint16_t id);

private:
	struct Action;
	std::vector<Action>   mActions;
	std::vector<uint16_t> mExecuteQueue;
	unsigned              mJobIndex = 0;
};

} // namespace Wind