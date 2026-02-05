#include "Compositor.h"
#include "Config.h"
#include "GlProgram.h"
#include "Graphics.h"

#include <cassert>

namespace Wind {

// mBlur { engine.GetGraphics() }
UICompositor::UICompositor(Graphics& graphics, int width, int height)
	: mGraphics{graphics}
	, mFrameBuffers {
	    { width, height, FBOFlags::color },
	    { width, height, FBOFlags::color },
	    { width, height, FBOFlags::color },
    }
	, mTransition{ScreenTransition::none}
	, mAccumTime{0.f}
	, mCurrDst{0}{

	InitPrograms(graphics);
	//, mFrameBufferHalfRes { RefWindowWidth / 2, RefWindowHeight / 2, FBOFlags::color }

	PipelineState pipelineState;
	pipelineState.mDepthEnabled = false;
	pipelineState.mBlending = false;
	mPipelineHandle = mGraphics.NewPipeline(pipelineState);
}

const GlFrameBuffer& UICompositor::GetWriteableFramebuffer() const {
	return mFrameBuffers[mCurrDst];
}

void UICompositor::SetTransition(ScreenTransition newTransition) {
	if (newTransition != ScreenTransition::none) {
		// End current. Start new
		mTransition = newTransition;
		mAccumTime = 0.f;
	}
}

const GlFrameBuffer& UICompositor::Execute(float dt) {
	unsigned res = mCurrDst;
	if (mTransition != ScreenTransition::none) {
		res = 2;
		mGraphics.SetFrameBuffer(mFrameBuffers[2]);
	}

	float duration = 0.f;
	switch (mTransition) {
	case ScreenTransition::none:
		res = mCurrDst;
		mCurrDst = (mCurrDst + 1) & 1;
		break;
	case ScreenTransition::fade:
		Fade(mCurrDst, (mCurrDst + 1) & 1, mAccumTime / mFadeTransition.mDuration);
		duration = mFadeTransition.mDuration;
		break;
	case ScreenTransition::slideLeft:
		Slide(mCurrDst, (mCurrDst + 1) & 1, -1.f, 0.f);
		duration = mSlideTransition.mDuration;
		break;
	case ScreenTransition::slideRight:
		Slide(mCurrDst, (mCurrDst + 1) & 1, +1.f, 0.f);
		duration = mSlideTransition.mDuration;
		break;
	case ScreenTransition::slideTop:
		Slide(mCurrDst, (mCurrDst + 1) & 1, 0.f, -1.f);
		duration = mSlideTransition.mDuration;
		break;
	case ScreenTransition::slideBottom:
		Slide(mCurrDst, (mCurrDst + 1) & 1, 0.f, 1.f);
		duration = mSlideTransition.mDuration;
		break;
	case ScreenTransition::pixelate:
		duration = mPixelateTransition.mDuration;
		break;
	case ScreenTransition::dissolve:
		Dissolve(mCurrDst, (mCurrDst + 1) & 1);
		duration = mDissolveTransition.mDuration;
		break;
	case ScreenTransition::zoomInOut:
		Zoom(mCurrDst, (mCurrDst + 1) & 1, 1.f);
		duration = mZoomTransition.mDuration;
		break;
	default: {
		break;
	}
	}

	if (mTransition != ScreenTransition::none) {
		mAccumTime += dt;
		if (mAccumTime > duration) {
			// End transition
			mTransition = ScreenTransition::none;
			mAccumTime = 0.f;
		}
	}

	//	const GlFrameBuffer* mip[] = { &mFrameBufferHalfRes, &mFrameBufferQuarterRes };
	//	mBlur.Run(mFrameBuffer_0, mip, std::size(mip));

	return mFrameBuffers[res];
}

bool UICompositor::IsIdle() const {
	return mTransition == ScreenTransition::none;
}

void UICompositor::ConfigurePixelTransition(const PixelateTransition& settings) {
	mPixelateTransition = settings;
}

void UICompositor::InitPrograms(Graphics& graphics) {
	// TODO Register search paths
	InitProgramUniforms(mFadeProgram, SHADERS_FOLDER "transitions/fade.fs", graphics);
	InitProgramUniforms(mSlideProgram, SHADERS_FOLDER "transitions/slide.fs", graphics);
	InitProgramUniforms(mPixelateProgram, SHADERS_FOLDER "transitions/pixelate.fs", graphics);
	InitProgramUniforms(mDissolveProgram, SHADERS_FOLDER "transitions/dissolve.fs", graphics);
	InitProgramUniforms(mZoomProgram, SHADERS_FOLDER "transitions/zoom.fs", graphics);
}

void UICompositor::InitProgramUniforms(Program& programData, const char* fsPath, Graphics& graphics) {
	programData.mHandle = graphics.NewProgram(SHADERS_FOLDER "fullscreenTriangle.vs", fsPath);
	if (programData.mHandle != nullProgram) {
		const GlProgram& program = graphics.GetProgram(programData.mHandle);
		programData.mTexture0 = program.GetUniformLocation("texture0");
		programData.mTexture1 = program.GetUniformLocation("texture1");
		programData.color0 = program.GetUniformLocation("fadeColor");
		programData.mMisc = program.GetUniformLocation("misc");
		programData.mValid = (programData.mTexture0 != -1 && programData.mTexture1 != -1);
	}
	else {
		programData.mValid = false;
	}
}

void UICompositor::Fade(unsigned first, unsigned second, float progress) const {
	if (! mFadeProgram.mValid) {
		return;
	}
	const FadeTransition& settings = mFadeTransition;
	const int             uniformLocations[] = {
        mFadeProgram.mMisc,
        mFadeProgram.color0,
	};
	const Vec4 uniforms[] {
		{ progress, 0.f, 0.f, 0.f },
		(Vec4)(settings.mFadeColor),
	};
	Composite(mFadeProgram, first, second, uniformLocations, uniforms, std::size(uniformLocations));
}

void UICompositor::Slide(unsigned first, unsigned second, float dirx, float diry) const {
	if (! mSlideProgram.mValid) {
		return;
	}
	const SlideTransition& settings = mSlideTransition;
	const float            progress = mSlideTransition.mTimeCurve(mAccumTime / mSlideTransition.mDuration);
	const int              uniformLocations[] = {
        mSlideProgram.mMisc,
        mSlideProgram.color0,
	};
	const Vec4 uniforms[] {
		{ dirx, diry, progress, 2.f * settings.mBorderThickness / mGraphics.GetTargetWidth() },
		(Vec4)(settings.mBorderColor),
	};
	Composite(mSlideProgram, first, second, uniformLocations, uniforms, std::size(uniformLocations));
}

void UICompositor::Dissolve(unsigned first, unsigned second) const {
	if (! mDissolveProgram.mValid) {
		return;
	}
	const float progress = mAccumTime / mDissolveTransition.mDuration;
	const int   uniformLocations[] = {
        mDissolveProgram.mMisc,
	};
	const Vec4 uniforms[] {
		{ progress, 0.f, 0.f, 0.f },
	};
	Composite(mDissolveProgram, first, second, uniformLocations, uniforms, std::size(uniformLocations));
}

void UICompositor::Zoom(unsigned first, unsigned second, float dir) const {
	const float progress = mAccumTime / mZoomTransition.mDuration;
	const int   uniformLocations[] = {
        mZoomProgram.mMisc,
        //  mZoomProgram.color0,
	};
	const Vec4 uniforms[] {
		{ dir, progress, mZoomTransition.mZoomFactor, 0.f },
		{ 1.f, 1.f, 1.f, 1.f },
	};
	Composite(mZoomProgram, first, second, uniformLocations, uniforms, std::size(uniformLocations));
}

void UICompositor::Composite(const Program& program, unsigned first, unsigned second, const int uniformLocations[], const Vec4 uniforms[],
                             int numUniforms) const {
	mGraphics.SetPipeline(mPipelineHandle);

	const unsigned textureIds[] = {
		mFrameBuffers[first].GetColorAttachment(),
		mFrameBuffers[second].GetColorAttachment(),
	};

	// TODO SAmplers, border

	DrawCall drawCall;
	drawCall.uniformLocations = uniformLocations;
	drawCall.uniforms = uniforms;
	drawCall.numUniforms = numUniforms;
	drawCall.textures = textureIds;
	drawCall.numTextures = 2;
	drawCall.program = program.mHandle;
	drawCall.mesh = triangleMesh;
	drawCall.drawOrder = 0;
	mGraphics.Draw(drawCall);
}

} // namespace Wind