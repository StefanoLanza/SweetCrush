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
	, mCurrTransition{ScreenTransition::none}
	, mAccumTime{0.f}
	, mCurrDst{0}{

	// TODO Register search paths
	mProgram.mHandle = graphics.NewProgram(SHADERS_FOLDER "fullscreenTriangle.vs", SHADERS_FOLDER "composite.fs");
	if (mProgram.mHandle != nullProgram) {
		const GlProgram& program = graphics.GetProgram(mProgram.mHandle);
		mProgram.mTexture0 = program.GetUniformLocation("texture0");
		mProgram.mTexture1 = program.GetUniformLocation("texture1");
		mProgram.uvRect0 = program.GetUniformLocation("uvRect0");
		mProgram.uvRect1 = program.GetUniformLocation("uvRect1");
		mProgram.mProgress = program.GetUniformLocation("progress");
		mProgram.mValid = (mProgram.mTexture0 != -1 && mProgram.mTexture1 != -1 && mProgram.uvRect0 != -1 && mProgram.uvRect1 != -1);
	}

	//, mFrameBufferHalfRes { RefWindowWidth / 2, RefWindowHeight / 2, FBOFlags::color }
}

const GlFrameBuffer& Compositor::GetWriteableFramebuffer() const {
	return mFrameBuffers[mCurrDst];
}

const GlFrameBuffer& Compositor::Composite(ScreenTransition transition, float dt) {
	const float T = 1.f;
	if (transition == ScreenTransition::none) {
		// Wait for current to end
		if (mCurrTransition != ScreenTransition::none) {
			if (mAccumTime > T) {
				mCurrTransition = ScreenTransition::none;
				mAccumTime = 0.f;
			}
		}
	}
	else {
		// End current. Start new
		mCurrTransition = transition;
		mAccumTime = 0.f;
	}

	float progress = mAccumTime / T;
	mAccumTime += dt;

	unsigned res = mCurrDst;
	switch (mCurrTransition) {
	case ScreenTransition::none:
		res = mCurrDst;
		mCurrDst = (mCurrDst + 1) & 1;
		break;
	default:
		mGraphics.SetFrameBuffer(mFrameBuffers[2]);
		SlideIn(mCurrDst, (mCurrDst + 1) & 1, progress);
		res = 2;
		break;
	}

	//	const GlFrameBuffer* mip[] = { &mFrameBufferHalfRes, &mFrameBufferQuarterRes };
	//	mBlur.Run(mFrameBuffer_0, mip, std::size(mip));

	return mFrameBuffers[res];
}

void Compositor::SlideIn(unsigned first, unsigned second, float progress) const {
	const Program& program = mProgram;
	if (! program.mValid) {
		return;
	}

	// TODO Cache
	PipelineState pipelineState;
	pipelineState.mDepthEnabled = false;
	pipelineState.mBlending = false;
	PipelineHandle pipelineHandle = mGraphics.NewPipeline(pipelineState);
	mGraphics.SetPipeline(pipelineHandle);

	const int  uniforms[] = { program.uvRect0, program.uvRect1, program.mProgress };
	const Vec4 uniformData[] = {
		{ 0.f, 0.f, 1.f, 1.f },
		{ 0.f, 0.f, 1.f, 1.f },
		{ progress, 0.f, 0.f, 0.f },
	};
	const unsigned textureIds[] = {
		mFrameBuffers[first].GetColorAttachment(),
		mFrameBuffers[second].GetColorAttachment(),
	};

	DrawCall drawCall;
	drawCall.uniforms = uniforms;
	drawCall.uniformData = uniformData;
	drawCall.numUniforms = std::size(uniforms);
	drawCall.textures = textureIds;
	drawCall.numTextures = 2;
	drawCall.program = program.mHandle;
	drawCall.mesh = triangleMesh;
	drawCall.drawOrder = 0;
	mGraphics.Draw(drawCall);
}

} // namespace Wind