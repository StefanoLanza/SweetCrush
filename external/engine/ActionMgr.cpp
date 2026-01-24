#include "ActionMgr.h"
#include <algorithm>
#include <cassert>
#include <limits>

namespace Wind {

struct ActionMgr::Action {
	ActionFunc  func;
	float       duration;
	float       delay;
	float       t;
};

ActionMgr::ActionMgr()
    : mNumBlocking { 0 } {
}

ActionMgr::~ActionMgr() = default;

ActionId ActionMgr::AddAction(ActionFunc&& func, float delay) {
	return AddTimedAction(std::move(func), std::numeric_limits<float>::max(), delay);
}

ActionId ActionMgr::AddTimedAction(ActionFunc&& func, float duration, float delay) {
	assert(duration >= 0.f);
	mActions.push_back({
	    std::move(func),
	    duration,
	    delay,
	    0.f,
	});
	++mNumBlocking;
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
            if (res) {
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

} // namespace Wind