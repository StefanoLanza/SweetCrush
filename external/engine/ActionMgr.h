#pragma once

#include <functional>
#include <vector>

namespace Wind {

// Returns true if complete
using ActionFunc = std::function<bool(float dt, float t)>;
constexpr float actionUndefDuration = -1.f;

class ActionMgr {
public:
	ActionMgr();
	~ActionMgr();

	void AddAction(int* counter, float delay, float duration, ActionFunc&& func);
	void RunActions(float dt);
	void Clear();

private:
	struct Action;
	std::vector<Action> mActions;
};

} // namespace Wind
