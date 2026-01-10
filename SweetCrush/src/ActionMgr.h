#pragma once

#include <cstdint>
#include <functional>
#include <vector>

// Returns true if complete
using ActionFunc = std::function<bool(float dt, float t)>;

enum ActionFlags : uint8_t {
	blocking = 0,
	nonBlocking = 1,
};

class ActionMgr final {
public:
	ActionMgr();
	~ActionMgr();

	void AddAction(ActionFunc&& func, float delay = 0.f, ActionFlags flags = ActionFlags::blocking);
	void AddTimedAction(ActionFunc&& func, float duration, float delay = 0, ActionFlags flags = ActionFlags::blocking);
	void RunActions(float dt);
	void Clear();
	bool AnyRunning() const;
	bool AnyBlocking() const;

private:
	struct Action;
	std::vector<Action> mActions;
	unsigned            mNumBlocking;
};
