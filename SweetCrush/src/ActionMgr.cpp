#include "ActionMgr.h"
#include <algorithm>
#include <limits>
#include <cassert>

struct ActionMgr::Action {
	float      delay;
	float      duration;
	float      t;
	ActionFunc func;
};

ActionMgr::ActionMgr() = default;

ActionMgr::~ActionMgr() = default;

void ActionMgr::AddAction(float delay, ActionFunc&& func) {
	AddTimedAction(delay, std::numeric_limits<float>::max(), std::move(func));
}

void ActionMgr::AddTimedAction(float delay, float duration, ActionFunc&& func) {
	assert(duration >= 0.f);
	mActions.push_back({ delay, duration, 0.f, std::move(func) });
}

void ActionMgr::RunActions(float dt) {
	auto pred = [dt](Action& action) {
		bool res = false;
		action.delay -= dt;
		if (action.delay < 0.f) {
			action.t += dt;
			float t01 = std::clamp(action.t / action.duration, 0.f, 1.f);
			if (action.t < action.duration) {
				res = action.func(dt, t01);
			}
			else {
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
