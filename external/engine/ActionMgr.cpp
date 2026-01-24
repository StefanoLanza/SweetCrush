#include "ActionMgr.h"

#include <algorithm>
#include <cassert>
#include <limits>

namespace Wind {

constexpr uint16_t nullIndex = (uint16_t)-1;

struct ActionMgr::Action {
	ActionFunc func;
	float      duration;
	float      delay;
	float      t;
	uint16_t   pending; // itself and children
	uint16_t   parent;
	uint16_t   firstChild;
	uint16_t   sibling;
};

ActionMgr::ActionMgr() {
}

ActionMgr::~ActionMgr() = default;

ActionId ActionMgr::AddAction(ActionFunc&& func, float delay) {
	return AddTimedAction(std::move(func), std::numeric_limits<float>::max(), delay);
}

ActionId ActionMgr::AddTimedAction(ActionFunc&& func, float duration, float delay) {
	assert(duration >= 0.f);
	mActions.push_back({
	    .func = std::move(func),
	    .duration = duration,
	    .delay = delay,
	    .t = 0.f,
	    .pending = 1,
	    .parent = nullIndex,
	    .firstChild = nullIndex,
	    .sibling = nullIndex,
	});
	return ActionId{(uint16_t)(mActions.size() - 1)};
}

ActionId ActionMgr::AddContinuation(ActionId parentId, ActionFunc&& func) {
	ActionId actionId = AddAction(std::move(func));

	Action& parent = mActions[parentId.Get()];
	assert(parent.pending > 0);
	parent.pending++;
	mActions[actionId.Get()].parent = parentId.Get();
	// Add to linked list of children
	if (parent.firstChild == nullIndex) {
		parent.firstChild = actionId.Get();
	}
	else {
		uint16_t child = parent.firstChild;
		while (mActions[child].sibling != nullIndex) {
			child = mActions[child].sibling;
		}
		mActions[child].sibling = actionId.Get();
	}

	return actionId;
}

void ActionMgr::RunActions(float dt) {
	auto pred = [dt, this](Action& action) {
		bool res = false;
		action.delay -= dt;
		if (action.delay < 0.f) {
			action.t += dt;
			float t01 = std::clamp(action.t / action.duration, 0.f, 1.f);
			res = action.func(dt, t01);
			if (t01 >= 1.f) {
				res = true;
			}
			if (res) {
				--action.pending;
				if (action.parent != nullIndex) {
					--mActions[action.parent].pending;
				}
			}
		}
		return res;
	};
	// Run actions and remove those that have completed
	mActions.erase(std::remove_if(mActions.begin(), mActions.end(), pred), mActions.end());
}

bool ActionMgr::IsFinished(ActionId actionId) const {
	return mActions[actionId.Get()].pending == 0;
}

void ActionMgr::Clear() {
	mActions.clear();
}

bool ActionMgr::AnyRunning() const {
	return mActions.empty() == false;
}

} // namespace Wind