#include "ScreenManager.h"

#include <cassert>

namespace Wind {

ScreenManager::ScreenManager()
    : mCurr {}
    , mHistory {}
    , mHistorySize { 0 }
    , mTransition { ScreenTransition::none } {
}

void ScreenManager::Register(Screen* screen) {
	mScreens.push_back(screen);
}

void ScreenManager::SetMain(ScreenId id) {
	mCurr = id;
	mScreens[id.Get()]->Enter(ScreenNavArgs { id });
}

void ScreenManager::Tick(float dt, const Input& input) {
	const ScreenId    topScreenId = mCurr;
	Screen&           topScreen = *mScreens[topScreenId.Get()];
	const ScreenEvent event = topScreen.Tick(dt, input);
	bool              addToHistory = false;
	switch (event.mOp) {
	case ScreenOp::keep:
		mTransition = ScreenTransition::none;
		break;
	case ScreenOp::goTo: {
		topScreen.Exit();
		ScreenNavArgs navArgs { topScreenId };
		std::memcpy(navArgs.mParams, event.mParams, sizeof navArgs.mParams);
		mScreens[event.mNext.Get()]->Enter(navArgs);
		mCurr = event.mNext;
		mTransition = event.mTransition;
		addToHistory = true;
		break;
	}
	case ScreenOp::back: {
		assert(mHistorySize > 0);
		topScreen.Exit();
		ScreenId next = mHistory[mHistorySize - 1];
		--mHistorySize;
		ScreenNavArgs navArgs { topScreenId };
		std::memcpy(navArgs.mParams, event.mParams, sizeof navArgs.mParams);
		mScreens[next.Get()]->Enter(navArgs);
		mCurr = next;
		mTransition = event.mTransition;
		break;
	}
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

void ScreenManager::Draw(UIRenderer& uiRenderer, float dt) const {
	// TODO Composition
	// two framebuffers A, B
	// no event: draw to A, blit A
	// event: A is snapshot of pervious screen. draw to B, composite A into B, blit B
	// A must be preserved, B is dynamic

	mScreens[mCurr.Get()]->Draw(uiRenderer, dt);
}

bool ScreenManager::CanGoBack() const {
	return mHistorySize > 1;
}

void ScreenManager::GoBack() {
}

void ScreenManager::GoForward() {
}

ScreenTransition ScreenManager::GetTransition() const {
	return mTransition;
}

} // namespace Wind