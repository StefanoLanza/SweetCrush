#pragma once

#include "Color.h"
#include "Easings.h"
#include "FwdDecl.h"
#include "GlFrameBuffer.h"
#include "Screen.h"

namespace Wind {

struct PixelateTransition {
	float mDuration = 0.25f;
	Color mDissolveColor = whiteColor;
};

struct SlideTransition {
	float mDuration = 0.25f;
	float (*mTimeCurve)(float) = EaseInQuad;
	Color mBorderColor = whiteColor;
	float mBorderThickness = 8.f; // pixels
};

struct DissolveTransition {
	float mDuration = 0.25f;
};

class Compositor {
public:
	explicit Compositor(Graphics& graphics, int width, int height);
	const GlFrameBuffer& GetWriteableFramebuffer() const;
	void                 SetTransition(ScreenTransition newTransition);
	const GlFrameBuffer& Execute(float dt);
	bool                 IsIdle() const;
	void                 ConfigurePixelTransition(const PixelateTransition& settings);

private:
	struct Program;
	void InitPrograms(Graphics& graphics);
	void InitProgramUniforms(Program& program, const char* fsPath, Graphics& graphics);
	void Slide(unsigned first, unsigned second, float dir) const;
	void Dissolve(unsigned first, unsigned second) const;
	void Composite(const Program& program, unsigned first, unsigned second, const int uniformLocations[], const Vec4 uniforms[],
	               int numUniforms) const;

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
		GLint         color0;
		GLint         mMisc;
		bool          mValid;
	};
	PipelineHandle     mPipelineHandle;
	Program            mSlideProgram;
	Program            mPixelateProgram;
	Program            mDissolveProgram;
	PixelateTransition mPixelateTransition;
	SlideTransition    mSlideTransition;
	DissolveTransition mDissolveTransition;
};

} // namespace Wind
