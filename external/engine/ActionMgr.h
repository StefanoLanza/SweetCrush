#pragma once

#include <cstdint>
#include <functional>
#include <vector>

namespace Wind {

// Returns true if complete
using ActionFunc = std::function<bool(float dt, float t)>;

enum class ActionId : uint32_t;

struct ActionInfo {
	float       duration = -1;
	float       delay = 0;
};

class ActionMgr final {
public:
	ActionMgr();
	~ActionMgr();

	ActionId AddAction(ActionFunc&& func, float delay = 0.f);
	ActionId AddTimedAction(ActionFunc&& func, float duration, float delay = 0);
	void     RunActions(float dt);
	void     Clear();
	bool     AnyRunning() const;
	bool     AnyBlocking() const;

private:
	struct Action;
	std::vector<Action> mActions;
	unsigned            mNumBlocking;
};

} // namespace Wind