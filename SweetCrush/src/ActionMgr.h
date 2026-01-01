#pragma once

#include <functional>
#include <vector>

// Returns true if complete
using ActionFunc = std::function<bool(float dt, float t)>;

class ActionMgr final {
public:
	ActionMgr();
	~ActionMgr();

	void AddAction(float delay, ActionFunc&& func);
	void AddTimedAction(float delay, float duration, ActionFunc&& func);
	void RunActions(float dt);
	void Clear();
	bool AnyRunning() const;

private:
	struct Action;
	std::vector<Action> mActions;
};
