#include "ActionMgr.h"

#include <algorithm>
#include <cassert>
#include <limits>

namespace Wind {

constexpr uint16_t nullIndex = (uint16_t)-1;
constexpr unsigned poolSize = 256;

struct ActionMgr::Action {
	ActionFunc func;
	float      duration;
	float      delay;
	float      t;
	uint16_t   pending; // itself and children
	uint16_t   parent;
	uint16_t   next;
	uint16_t   continuation;
};

ActionMgr::ActionMgr() {
	mActions.resize(poolSize); // init pool
}

ActionMgr::~ActionMgr() = default;

ActionId ActionMgr::AddAction(ActionFunc&& func, float delay) {
	ActionId id = NewAction(std::move(func), std::numeric_limits<float>::max(), delay);
	PushAction(id.Get());
	return id;
}

ActionId ActionMgr::AddTimedAction(ActionFunc&& func, float duration, float delay) {
	assert(duration >= 0.f);
	ActionId id = NewAction(std::move(func), duration, delay);
	PushAction(id.Get());
	return id;
}

ActionId ActionMgr::AddChild(ActionId parentId, ActionFunc&& func) {
	ActionId actionId = AddAction(std::move(func));

	Action& parent = mActions[parentId.Get()];
	assert(parent.pending > 0);
	parent.pending++;
	mActions[actionId.Get()].parent = parentId.Get();
#if 0
	// Add to linked list of children
	if (parent.firstChild == nullIndex) {
		parent.firstChild = actionId.Get();
	}
	else {
		uint16_t child = parent.firstChild;
		while (mActions[child].next != nullIndex) {
			child = mActions[child].next;
		}
		mActions[child].next = actionId.Get();
	}
#endif

	return actionId;
}

ActionId ActionMgr::AddContinuation(ActionId parentId, ActionFunc&& func, float duration) {
	Action& prev = mActions[parentId.Get()];
	assert(prev.continuation == nullIndex);

	ActionId id = NewAction(std::move(func), duration, 0.f);

	// Add continuation to linked list
	if (prev.continuation == nullIndex) {
		prev.continuation = id.Get();
	}
	else {
		uint16_t next = prev.continuation;
		while (mActions[next].next != nullIndex) {
			next = mActions[next].next;
		}
		mActions[next].next = id.Get();
	}

	return id;
}

void ActionMgr::Execute(float dt) {
	auto pred = [](Action& action, float dt) {
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
	for (size_t i = 0; i < mExecuteQueue.size();) {
		// Run action
		auto    id = mExecuteQueue[i];
		Action& action = mActions[id];
		bool    finished = pred(action, dt);
		if (finished) {
			assert(action.pending > 0);
			--action.pending;
			// Inform prev
			if (action.parent != nullIndex) {
				--mActions[action.parent].pending;
			}
			// Remove from queue
			mExecuteQueue[i] = mExecuteQueue.back();
			mExecuteQueue.pop_back();
			// ::erase(mExecuteQueue,

			// Enqueue continuations
			uint16_t next = action.continuation;
			while (next != nullIndex) {
				PushAction(next);
				next = mActions[next].next;
			}
		}
		else {
			// Continue running
			++i;
		}
	}
}

bool ActionMgr::IsFinished(ActionId actionId) const {
	// TODO how to detect recycling
	return mActions[actionId.Get()].pending == 0;
}

void ActionMgr::Clear() {
	mExecuteQueue.clear();
}

bool ActionMgr::AnyRunning() const {
	return mExecuteQueue.empty() == false;
}

ActionId ActionMgr::NewAction(ActionFunc&& func, float duration, float delay) {
	Action& action = mActions[mJobIndex];
	assert(action.pending == 0 && "Job queue is full");

	action = Action {
		.func = std::move(func),
		.duration = duration,
		.delay = delay,
		.t = 0.f,
		.pending = 1,
		.parent = nullIndex,
		.next = nullIndex,
		.continuation = nullIndex,
	};
	ActionId id { (uint16_t)mJobIndex };
	mJobIndex = (mJobIndex + 1) % poolSize;
	return id;
}

void ActionMgr::PushAction(uint16_t index) {
	mExecuteQueue.push_back(index);
}

} // namespace Wind