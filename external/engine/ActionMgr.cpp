#include "ActionMgr.h"

#include <algorithm>
#include <cassert>
#include <limits>

namespace Wind {

namespace {

constexpr uint16_t nullIndex = (uint16_t)-1;
constexpr unsigned poolSize = 256;

} // namespace

struct ActionMgr::Action {
	ActionFunc func;
	int*       counter;
	float      duration;
	float      delay;
	float      t;
	uint16_t   next;
	uint16_t   continuation;
};

ActionMgr::ActionMgr() {
	mActions.resize(poolSize); // init pool
	// Build free list
	mFreeIndex = 0;
	for (size_t i = 0; i < poolSize - 1; ++i) {
		mActions[i].next = (uint16_t)(i + 1);
	}
	mActions.back().next = nullIndex;
}

ActionMgr::~ActionMgr() = default;

ActionId ActionMgr::AddAction(ActionFunc&& func, float delay) {
	ActionId id = NewAction(std::move(func), std::numeric_limits<float>::max(), delay, nullptr);
	PushAction(id.Get());
	return id;
}

ActionId ActionMgr::AddTimedAction(ActionFunc&& func, float duration, float delay) {
	assert(duration >= 0.f);
	ActionId id = NewAction(std::move(func), duration, delay, nullptr);
	PushAction(id.Get());
	return id;
}

ActionId ActionMgr::AddAction(ActionFunc&& func, const ActionDesc& desc) {
	ActionId id = NewAction(std::move(func), desc.duration, desc.delay, desc.counter);
	PushAction(id.Get());
	return id;
}

ActionId ActionMgr::AddContinuation(ActionId parentId, ActionFunc&& func, float duration) {
	Action& prev = mActions[parentId.Get()];
	assert(prev.continuation == nullIndex);

	ActionId id = NewAction(std::move(func), duration, 0.f, nullptr);

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
		auto    index = mExecuteQueue[i];
		Action& action = mActions[index];
		bool    finished = pred(action, dt);
		if (finished) {
			if (action.counter) {
				--(*action.counter); // inform user
			}
			action.func = nullptr;

			// Remove from queue
			mExecuteQueue[i] = mExecuteQueue.back();
			mExecuteQueue.pop_back();

			// Enqueue continuations
			uint16_t next = action.continuation;
			while (next != nullIndex) {
				PushAction(next);
				next = mActions[next].next;
			}

			FreeAction(index);
		}
		else {
			// Continue running
			++i;
		}
	}
}

void ActionMgr::Clear() {
	mExecuteQueue.clear();
}

bool ActionMgr::AnyRunning() const {
	return mExecuteQueue.empty() == false;
}

ActionId ActionMgr::NewAction(ActionFunc&& func, float duration, float delay, int* counter) {
	assert(mFreeIndex != nullIndex && "Action pool is full");
	Action& action = mActions[mFreeIndex];

	ActionId id { mFreeIndex };
	mFreeIndex = action.next; // before overriding action!

	if (counter) {
		++(*counter);
	}

	action = Action {
		.func = std::move(func),
		.counter = counter,
		.duration = duration,
		.delay = delay,
		.t = 0.f,
		.next = nullIndex,
		.continuation = nullIndex,
	};
	return id;
}

void ActionMgr::PushAction(uint16_t index) {
	mExecuteQueue.push_back(index);
}

void ActionMgr::FreeAction(uint16_t index) {
	mActions[index].next = mFreeIndex;
	mFreeIndex = index;
}

} // namespace Wind