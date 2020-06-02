#include "ActionMgr.h"
#include <algorithm>

namespace Wind {

struct ActionMgr::Action {
	int*       counter;
	float      delay;
	float      duration;
	float      t;
	ActionFunc func;
};

ActionMgr::ActionMgr() = default;

ActionMgr::~ActionMgr() = default;

void ActionMgr::AddAction(int* counter, float delay, float duration, ActionFunc&& func) {
	if (counter) {
		++*counter;
	}
	if (duration <= 0.f) {
		duration = std::numeric_limits<float>::max();
	}
	mActions.push_back({ counter, delay, duration, 0.f, std::move(func) });
}

void ActionMgr::RunActions(float dt) {
	auto pred = [dt](Action& action) {
		bool res = false;
		action.delay -= dt;
		if (action.delay < 0.f) {
			action.t += dt;
			float t01 = std::clamp(action.t / action.duration, 0.f, 1.f);
			res = action.func(dt, t01);
			res = res || (action.t >= action.duration);
			if (res && action.counter) {
				--*action.counter; // Decrease counter, to inform client
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

} // namespace Wind
