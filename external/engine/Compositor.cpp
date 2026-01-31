#include "Compositor.h"
#include "Config.h"
#include "GlProgram.h"
#include "Graphics.h"

#include <cassert>

namespace Wind {

const float T = 0.25f;

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

	// TODO Register search paths
	mProgram.mHandle = graphics.NewProgram(SHADERS_FOLDER "fullscreenTriangle.vs", SHADERS_FOLDER "composite.fs");
	if (mProgram.mHandle != nullProgram) {
		const GlProgram& program = graphics.GetProgram(mProgram.mHandle);
		mProgram.mTexture0 = program.GetUniformLocation("texture0");
		mProgram.mTexture1 = program.GetUniformLocation("texture1");
		mProgram.uvRect0 = program.GetUniformLocation("uvRect0");
		mProgram.uvRect1 = program.GetUniformLocation("uvRect1");
		mProgram.color0 = program.GetUniformLocation("color0");
		mProgram.color1 = program.GetUniformLocation("color1");
		mProgram.mProgress = program.GetUniformLocation("progress");
		mProgram.mValid = (mProgram.mTexture0 != -1 && mProgram.mTexture1 != -1 && mProgram.uvRect0 != -1 && mProgram.uvRect1 != -1);
	}

	//, mFrameBufferHalfRes { RefWindowWidth / 2, RefWindowHeight / 2, FBOFlags::color }
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
	const float progress = mAccumTime / T;

	Vec4     uniforms[4];
	unsigned res = mCurrDst;
	switch (mTransition) {
	case ScreenTransition::none:
		res = mCurrDst;
		mCurrDst = (mCurrDst + 1) & 1;
		break;
	case ScreenTransition::slideIn: {
		res = 2;
		uniforms[0] = { 1.f - progress, 0.f, 1.f, 1.f };
		uniforms[1] = { 0.f, 0.f, 1.f, 1.f };
		uniforms[2] = { progress, 0.f, 0.f, 0.f }, mGraphics.SetFrameBuffer(mFrameBuffers[2]);
		Composite(mCurrDst, (mCurrDst + 1) & 1, uniforms, progress);
		break;
	}
	case ScreenTransition::slideOut: {
		res = 2;
		uniforms[0] = { -1.f + progress, 0.f, 1.f, 1.f };
		uniforms[1] = { 0.f, 0.f, 1.f, 1.f };
		uniforms[2] = { progress, 0.f, 0.f, 0.f }, mGraphics.SetFrameBuffer(mFrameBuffers[2]);
		Composite(mCurrDst, (mCurrDst + 1) & 1, uniforms, progress);
		break;
	}
	default: {
		res = 2;
		break;
	}
	}

	if (mTransition != ScreenTransition::none) {
		mAccumTime += dt;
		if (mAccumTime > T) {
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

void Compositor::Composite(unsigned first, unsigned second, const Vec4 uniformData[], float progress) const {
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

	const int      uniforms[] = { program.uvRect0, program.uvRect1, program.mProgress };
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