#pragma once

#include "Color.h"
#include "Easings.h"
#include "FwdDecl.h"
#include "GlFrameBuffer.h"
#include "Transition.h"

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

struct ZoomTransition {
	float mDuration = 0.25f;
	float (*mTimeCurve)(float) = EaseInQuad;
	Color mFadeColor = whiteColor;
};

struct DissolveTransition {
	float mDuration = 0.25f;
};

class UICompositor {
public:
	explicit UICompositor(Graphics& graphics, int width, int height);
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
	void Zoom(unsigned first, unsigned second, float dir) const;
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
		GLint         mTexture0 = -1;
		GLint         mTexture1 = -1;
		GLint         color0 = -1;
		GLint         mMisc = -1;
		bool          mValid = false;
	};
	PipelineHandle     mPipelineHandle;
	Program            mSlideProgram;
	Program            mPixelateProgram;
	Program            mDissolveProgram;
	Program            mZoomProgram;
	PixelateTransition mPixelateTransition;
	SlideTransition    mSlideTransition;
	DissolveTransition mDissolveTransition;
	ZoomTransition     mZoomTransition;
};

} // namespace Wind
