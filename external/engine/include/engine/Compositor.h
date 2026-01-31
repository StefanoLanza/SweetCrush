#pragma once

#include "FwdDecl.h"
#include "GlFrameBuffer.h"
#include "Screen.h"

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
	void SlideIn(unsigned first, unsigned second, float progress) const;

private:
	Graphics&        mGraphics;
	GlFrameBuffer    mFrameBuffers[3];
	ScreenTransition mCurrTransition;
	//	Wind::GlFrameBuffer           mFrameBufferHalfRes;
	// Wind::GlFrameBuffer           mFrameBufferQuarterRes;
	//	KawaseBlur              mBlur;
	float    mAccumTime;
	unsigned mCurrDst;
	struct Program {
		ProgramHandle mHandle;
		GLint         mTexture0;
		GLint         mTexture1;
		GLint         uvRect0;
		GLint         uvRect1;
		GLint         mProgress;
		bool          mValid;
	};
	Program mProgram;
};

} // namespace Wind
