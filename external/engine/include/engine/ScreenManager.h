#pragma once

#include "FwdDecl.h"
#include "Screen.h"

#include <vector>

namespace Wind {

class Input;
class UIRenderer;

class ScreenManager {
public:
	ScreenManager();
	void             Register(Screen* screen);
	void             SetMain(ScreenId id);
	ScreenTransition             Tick(float dt, const Input& input);
	void             Draw(UIRenderer& uiRenderer, float dt) const;
	bool             CanGoBack() const;
	void             GoBack();
	void             GoForward();
	// void AddEvent(const ScreenEvent& event); // TODO

private:
	std::vector<Screen*> mScreens;
	ScreenId             mCurr;
	ScreenId             mHistory[8];
	size_t               mHistorySize;
};

} // namespace Wind
