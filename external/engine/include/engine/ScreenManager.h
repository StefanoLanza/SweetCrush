#pragma once

#include "Screen.h"

#include <vector>

namespace Wind {

class Input;
class UIRenderer;

class ScreenManager {
public:
	ScreenManager();
	void Register(Screen* screen);
	void SetMain(ScreenId id);
	void Tick(float dt, const Input& input);
	void Draw(UIRenderer& uiRenderer) const;
	bool CanGoBack() const;
	void GoBack();
	void GoForward();
	void AddEvent(const ScreenEvent& event);

private:
	std::vector<Screen*> mScreens;
	ScreenId mStack[4];
	size_t   mStackSize;
	ScreenId mHistory[8];
	size_t   mHistorySize;
};

} // namespace Wind
