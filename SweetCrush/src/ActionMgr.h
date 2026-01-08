#pragma once

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

	void AddAction(ActionFunc&& func, float delay, ActionFlags flags = ActionFlags::blocking);
	void AddTimedAction(ActionFunc&& func, float delay, float duration, ActionFlags flags = ActionFlags::blocking);
	void RunActions(float dt);
	void Clear();
	bool AnyRunning() const;

private:
	struct Action;
	std::vector<Action> mActions;
};
