#pragma once

#include "GlFrameBuffer.h"
#include "Screen.h"
// #include "Kawase.h" // FIXME

#include <vector>

namespace Wind {

class Input;
class UIRenderer;
class Graphics;

class Compositor {
public:
	explicit Compositor(Graphics& graphics, int width, int height);
	const GlFrameBuffer& GetWriteableFramebuffer() const;
	const GlFrameBuffer& Composite(ScreenTransition transition, float dt);

private:
	Graphics&        mGraphics;
	GlFrameBuffer    mFrameBuffers[3];
	ScreenTransition mCurrTransition;
	//	Wind::GlFrameBuffer           mFrameBufferHalfRes;
	// Wind::GlFrameBuffer           mFrameBufferQuarterRes;
	//	KawaseBlur              mBlur;
	float    mAccumTime;
	unsigned mCurrDst;
};

class ScreenManager {
public:
	ScreenManager();
	void Register(Screen* screen);
	void SetMain(ScreenId id);
	void Tick(float dt, const Input& input);
	void Draw(UIRenderer& uiRenderer, float dt) const;
	bool CanGoBack() const;
	void GoBack();
	void GoForward();
	// void AddEvent(const ScreenEvent& event); // TODO

private:
	std::vector<Screen*> mScreens;
	ScreenId             mCurr;
	ScreenId             mHistory[8];
	size_t               mHistorySize;
};

} // namespace Wind
