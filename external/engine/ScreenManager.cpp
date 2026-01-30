#include "ScreenManager.h"

#include <cassert>

namespace Wind {

// mBlur { engine.GetGraphics() }
Compositor::Compositor(Graphics& graphics, int width, int height)
	: mGraphics{graphics}
	, mFrameBuffers {
	    { width, height, FBOFlags::color },
	    { width, height, FBOFlags::color },
	    { width, height, FBOFlags::color },
    }
	, mCurrTransition{ScreenTransition::none}
	, mAccumTime{0.f}
	, mCurrDst{0}{
	//, mFrameBufferHalfRes { RefWindowWidth / 2, RefWindowHeight / 2, FBOFlags::color }
}

const GlFrameBuffer& Compositor::GetWriteableFramebuffer() const {
	return mFrameBuffers[mCurrDst];
}

const GlFrameBuffer& Compositor::Composite(ScreenTransition transition, float dt) {
	mAccumTime += dt;
	mCurrTransition = transition;
	unsigned dst = mCurrDst;
	switch (transition) {
	case ScreenTransition::none:
		dst = mCurrDst;
		mAccumTime = 0.f;
		break;
	default:
		break; // TODO
	}

//	const GlFrameBuffer* mip[] = { &mFrameBufferHalfRes, &mFrameBufferQuarterRes };
//	mBlur.Run(mFrameBuffer_0, mip, std::size(mip));

	mCurrDst = (mCurrDst + 1) & 1;
	return mFrameBuffers[dst];
}

ScreenManager::ScreenManager()
    : mCurr {}
    , mHistory {}
    , mHistorySize { 0 } {
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
	const ScreenEvent transition = topScreen.Tick(dt, input);
	bool              addToHistory = false;
	switch (transition.mOp) {
	case ScreenOp::keep:
		break;
	case ScreenOp::goTo: {
		topScreen.Exit();
		ScreenNavArgs navArgs { topScreenId };
		std::memcpy(navArgs.mParams, transition.mParams, sizeof navArgs.mParams);
		mScreens[transition.mNext.Get()]->Enter(navArgs);
		mCurr = transition.mNext;
		addToHistory = true;
		break;
	}
	case ScreenOp::back: {
		assert(mHistorySize > 0);
		topScreen.Exit();
		ScreenId next = mHistory[mHistorySize - 1];
		--mHistorySize;
		ScreenNavArgs navArgs { topScreenId };
		std::memcpy(navArgs.mParams, transition.mParams, sizeof navArgs.mParams);
		mScreens[next.Get()]->Enter(navArgs);
		mCurr = next;
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
	// no transition: draw to A, blit A
	// transition: A is snapshot of pervious screen. draw to B, composite A into B, blit B
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

} // namespace Wind