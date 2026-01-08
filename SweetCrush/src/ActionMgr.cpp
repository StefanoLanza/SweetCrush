#include "ActionMgr.h"
#include <algorithm>
#include <cassert>
#include <limits>

struct ActionMgr::Action {
	ActionFunc  func;
	float       delay;
	float       duration;
	float       t;
	ActionFlags flags;
};

ActionMgr::ActionMgr() = default;

ActionMgr::~ActionMgr() = default;

void ActionMgr::AddAction(ActionFunc&& func, float delay, ActionFlags flags) {
	AddTimedAction(std::move(func), delay, std::numeric_limits<float>::max(), flags);
}

void ActionMgr::AddTimedAction(ActionFunc&& func, float delay, float duration, ActionFlags flags) {
	assert(duration >= 0.f);
	mActions.push_back({
	    std::move(func),
	    delay,
	    duration,
	    0.f,
		flags,
	});
}

void ActionMgr::RunActions(float dt) {
	auto pred = [dt](Action& action) {
		bool res = false;
		action.delay -= dt;
		if (action.delay < 0.f) {
			action.t += dt;
			float t01 = std::clamp(action.t / action.duration, 0.f, 1.f);
			res = action.func(dt, t01);
			if (t01 >= 1.f) {
				res = true;
			}
		}
		return res;
	};
	// Run actions and remove those that have completed
	mActions.erase(std::remove_if(mActions.begin(), mActions.end(), pred), mActions.end());
}

void ActionMgr::Clear() {
	mActions.clear();
}

bool ActionMgr::AnyRunning() const {
	return mActions.empty() == false;
}
