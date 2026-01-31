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
	void                 SetTransition(ScreenTransition newTransition);
	const GlFrameBuffer& Execute(float dt);
	bool                 IsIdle() const;

private:
	void Composite(unsigned first, unsigned second, const Vec4 uniforms[], float progress) const;

private:
	Graphics&        mGraphics;
	GlFrameBuffer    mFrameBuffers[3];
	ScreenTransition mTransition;
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
		GLint         color0;
		GLint         color1;
		GLint         mProgress;
		bool          mValid;
	};
	Program mProgram;
};

} // namespace Wind
