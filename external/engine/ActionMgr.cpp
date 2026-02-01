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
	mActionPool.resize(poolSize); // init pool
	// Build free list
	mFreeIndex = 0;
	for (size_t i = 0; i < poolSize - 1; ++i) {
		mActionPool[i].next = (uint16_t)(i + 1);
	}
	mActionPool.back().next = nullIndex;
}

ActionMgr::~ActionMgr() = default;

ActionId ActionMgr::AddAction(ActionFunc&& func, const ActionDesc& desc) {
	ActionId id = NewAction(std::move(func), desc.duration, desc.delay, desc.counter);
	PushAction(id.Get());
	return id;
}

ActionId ActionMgr::AddContinuation(ActionId parentId, ActionFunc&& func, float duration) {
	Action& prev = mActionPool[parentId.Get()];
	assert(prev.continuation == nullIndex);

	ActionId id = NewAction(std::move(func), duration, 0.f, nullptr);

	// Add continuation to linked list
	if (prev.continuation == nullIndex) {
		prev.continuation = id.Get();
	}
	else {
		uint16_t next = prev.continuation;
		while (mActionPool[next].next != nullIndex) {
			next = mActionPool[next].next;
		}
		mActionPool[next].next = id.Get();
	}

	return id;
}

void ActionMgr::Run(float dt) {
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
	for (size_t i = 0; i < mRunQueue.size();) {
		auto    index = mRunQueue[i];
		Action& action = mActionPool[index];
		bool    finished = pred(action, dt);
		if (finished) {
			if (action.counter) {
				--(*action.counter); // inform user
			}
			action.func = nullptr;

			// Remove from queue
			mRunQueue[i] = mRunQueue.back();
			mRunQueue.pop_back();

			// Enqueue continuations
			uint16_t next = action.continuation;
			while (next != nullIndex) {
				PushAction(next);
				next = mActionPool[next].next;
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
	mRunQueue.clear();
}

bool ActionMgr::AnyRunning() const {
	return mRunQueue.empty() == false;
}

ActionId ActionMgr::NewAction(ActionFunc&& func, float duration, float delay, int* counter) {
	assert(mFreeIndex != nullIndex && "Action pool is full");
	Action& action = mActionPool[mFreeIndex];

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
	mRunQueue.push_back(index);
}

void ActionMgr::FreeAction(uint16_t index) {
	mActionPool[index].next = mFreeIndex;
	mFreeIndex = index;
}

} // namespace Wind