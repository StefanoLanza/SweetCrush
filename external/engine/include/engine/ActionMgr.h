#pragma once

#include "Id.h"

#include <cstdint>
#include <functional>
#include <vector>

namespace Wind {

struct ActionTag;
using ActionId = Id<ActionTag, uint16_t, (uint16_t)-1>;

// Returns true if complete
using ActionFunc = std::function<bool(float dt, float t)>;

struct ActionDesc {
	float duration = -1;
	float delay = 0;
	int*  counter = nullptr;
};

class ActionMgr final {
public:
	ActionMgr();
	~ActionMgr();

	ActionId AddAction(ActionFunc&& func, const ActionDesc& desc);
	ActionId AddContinuation(ActionId parentId, ActionFunc&& func, float duration);
	void     Run(float dt);
	void     Clear();
	bool     AnyRunning() const;

private:
	ActionId NewAction(ActionFunc&& func, float duration, float delay, int* counter);
	void     PushAction(uint16_t index);
	void     FreeAction(uint16_t index);

private:
	struct Action;
	std::vector<Action>   mActionPool;
	std::vector<uint16_t> mRunQueue;
	uint16_t              mFreeIndex;
};

} // namespace Wind