#include "ScreenManager.h"

#include <cassert>

namespace Wind {

ScreenManager::ScreenManager()
    : mStack {}
    , mStackSize { 0 }
    , mHistory {}
    , mHistorySize { 0 } {
}

void ScreenManager::Register(Screen* screen) {
	mScreens.push_back(screen);
}

void ScreenManager::SetMain(ScreenId id) {
	mStack[0] = id;
	mStackSize = 1;
	mScreens[id.Get()]->Enter(id, nullptr);
}

void ScreenManager::Tick(float dt, const Input& input) {
	if (mStackSize == 0) {
		// Call SetMain
		return;
	}
	const ScreenId         topScreenId = mStack[mStackSize - 1];
	Screen&                topScreen = *mScreens[topScreenId.Get()];
	const ScreenEvent transition = topScreen.Tick(dt, input);
	bool                   addToHistory = false;
	switch (transition.mOp) {
	case ScreenOp::keep:
		break;
	case ScreenOp::goTo:
		for (size_t i = 0; i < mStackSize; ++i) {
			mScreens[mStack[i].Get()]->Exit();
		}
		mScreens[transition.mNext.Get()]->Enter(topScreenId, transition.mParams);
		mStack[0] = transition.mNext;
		mStackSize = 1;
		addToHistory = true;
		break;
	case ScreenOp::back:
		assert(mHistorySize > 0);
		for (size_t i = 0; i < mStackSize; ++i) {
			mScreens[mStack[i].Get()]->Exit();
		}
		ScreenId next = mHistory[mHistorySize - 1];
		--mHistorySize;
		mScreens[next.Get()]->Enter(topScreenId, transition.mParams);
		mStack[0] = next;
		mStackSize = 1;
		break;
	};
	if (addToHistory) {
		if (mHistorySize == std::size(mHistory)) {
			// Remove oldest entry
			for (size_t i = 0; i < std::size(mHistory) - 1; ++i) {
				mHistory[i] = mHistory[i + 1];
			}
		}
		mHistory[mHistorySize] = topScreenId;
		++mHistorySize;
	}
}

void ScreenManager::Draw(UIRenderer& uiRenderer) const {
	mScreens[mStack[mStackSize - 1].Get()]->Draw(uiRenderer);
}

bool ScreenManager::CanGoBack() const {
	return mStackSize > 1;
}

void ScreenManager::GoBack() {
}

void ScreenManager::GoForward() {
}

} // namespace Wind