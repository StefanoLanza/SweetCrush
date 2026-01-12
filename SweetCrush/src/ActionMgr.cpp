#include "ActionMgr.h"
#include <algorithm>
#include <cassert>
#include <limits>

struct ActionMgr::Action {
	ActionFunc  func;
	float       duration;
	float       delay;
	float       t;
	ActionFlags flags;
};

ActionMgr::ActionMgr()
    : mNumBlocking { 0 } {
}

ActionMgr::~ActionMgr() = default;

ActionId ActionMgr::AddAction(ActionFunc&& func, float delay, ActionFlags flags) {
	return AddTimedAction(std::move(func), std::numeric_limits<float>::max(), delay, flags);
}

ActionId ActionMgr::AddTimedAction(ActionFunc&& func, float duration, float delay, ActionFlags flags) {
	assert(duration >= 0.f);
	mActions.push_back({
	    std::move(func),
	    duration,
	    delay,
	    0.f,
	    flags,
	});
	if (flags & ActionFlags::blocking) {
		++mNumBlocking;
	}
	return static_cast<ActionId>(mActions.size());
}

void ActionMgr::RunActions(float dt) {
	auto& numBlocking = mNumBlocking;
	auto  pred = [dt, &numBlocking](Action& action) {
        bool res = false;
        action.delay -= dt;
        if (action.delay < 0.f) {
            action.t += dt;
            float t01 = std::clamp(action.t / action.duration, 0.f, 1.f);
            res = action.func(dt, t01);
            if (t01 >= 1.f) {
                res = true;
            }
            if (res && (action.flags & ActionFlags::blocking)) {
				assert(numBlocking > 0);
                --numBlocking;
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

bool ActionMgr::AnyBlocking() const {
	return mNumBlocking != 0;
}