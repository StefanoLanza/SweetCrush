#include "Compositor.h"
#include "Config.h"
#include "GlProgram.h"
#include "Graphics.h"

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

const GlFrameBuffer& Compositor::GetWriteableFramebuffer() const {
	return mFrameBuffers[mCurrDst];
}

void Compositor::SetTransition(ScreenTransition newTransition) {
	if (newTransition != ScreenTransition::none) {
		// End current. Start new
		mTransition = newTransition;
		mAccumTime = 0.f;
	}
}

const GlFrameBuffer& Compositor::Execute(float dt) {
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
	case ScreenTransition::slideIn:
		Slide(mCurrDst, (mCurrDst + 1) & 1, -1.f);
		duration = mSlideTransition.mDuration;
		break;
	case ScreenTransition::slideOut:
		Slide(mCurrDst, (mCurrDst + 1) & 1, +1.f);
		duration = mSlideTransition.mDuration;
		break;
	case ScreenTransition::pixelate:
		duration = mPixelateTransition.mDuration;
		break;
	case ScreenTransition::dissolve:
		Dissolve(mCurrDst, (mCurrDst + 1) & 1);
		duration = mDissolveTransition.mDuration;
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

bool Compositor::IsIdle() const {
	return mTransition == ScreenTransition::none;
}

void Compositor::ConfigurePixelTransition(const PixelateTransition& settings) {
	mPixelateTransition = settings;
}

void Compositor::InitPrograms(Graphics& graphics) {
	// TODO Register search paths
	InitProgramUniforms(mSlideProgram, SHADERS_FOLDER "transitions/slide.fs", graphics);
	InitProgramUniforms(mPixelateProgram, SHADERS_FOLDER "transitions/pixelate.fs", graphics);
	InitProgramUniforms(mDissolveProgram, SHADERS_FOLDER "transitions/dissolve.fs", graphics);
}

void Compositor::InitProgramUniforms(Program& programData, const char* fsPath, Graphics& graphics) {
	programData.mHandle = graphics.NewProgram(SHADERS_FOLDER "fullscreenTriangle.vs", fsPath);
	if (programData.mHandle != nullProgram) {
		const GlProgram& program = graphics.GetProgram(programData.mHandle);
		programData.mTexture0 = program.GetUniformLocation("texture0");
		programData.mTexture1 = program.GetUniformLocation("texture1");
		programData.uvRect0 = program.GetUniformLocation("uvRect0");
		programData.uvRect1 = program.GetUniformLocation("uvRect1");
		programData.color0 = program.GetUniformLocation("color0");
		programData.color1 = program.GetUniformLocation("color1");
		programData.mProgress = program.GetUniformLocation("progress");
		programData.mValid = (programData.mTexture0 != -1 && programData.mTexture1 != -1);
	}
}

void Compositor::Slide(unsigned first, unsigned second, float dir) const {
	if (! mSlideProgram.mValid) {
		return;
	}
	const float progress = mSlideTransition.mTimeCurve(mAccumTime / mSlideTransition.mDuration);
	const int   uniformLocations[] = {
        mSlideProgram.uvRect0,
        mSlideProgram.uvRect1,
        mSlideProgram.mProgress,
	};
	const Vec4 uniforms[] {
		{ dir * (1.f - progress), 0.f, 1.f, 1.f },
		{ dir * (-progress), 0.f, 1.f, 1.f },
		{ progress, 0.f, 0.f, 0.f },
	};
	Composite(mSlideProgram, first, second, uniformLocations, uniforms, std::size(uniformLocations));
}

void Compositor::Dissolve(unsigned first, unsigned second) const {
	if (! mDissolveProgram.mValid) {
		return;
	}
	const float progress = mAccumTime / mDissolveTransition.mDuration;
	const int   uniformLocations[] = {
        mDissolveProgram.mProgress,
	};
	const Vec4 uniforms[] {
		{ progress, 0.f, 0.f, 0.f },
	};
	Composite(mDissolveProgram, first, second, uniformLocations, uniforms, std::size(uniformLocations));
}

void Compositor::Composite(const Program& program, unsigned first, unsigned second, const int uniformLocations[], const Vec4 uniforms[],
                           int numUniforms) const {
	mGraphics.SetPipeline(mPipelineHandle);

	const unsigned textureIds[] = {
		mFrameBuffers[first].GetColorAttachment(),
		mFrameBuffers[second].GetColorAttachment(),
	};

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