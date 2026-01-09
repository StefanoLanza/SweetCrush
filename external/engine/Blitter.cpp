#include "Blitter.h"
#include "Config.h"
#include "GlFrameBuffer.h"
#include "GlProgram.h"
#include "Graphics.h"

namespace Wind {

class Blitter::Impl {
public:
	explicit Impl(Graphics& graphics);
	void Blit(const GlFrameBuffer& frameBuffer) const;
	Vec2 WindowToFrameBuffer(Vec2 winCoord, const GlFrameBuffer& frameBuffer) const;

private:
	RectI ComputeTargetRect(const GlFrameBuffer& frameBuffer) const;

private:
	Graphics&     mGraphics;
	ProgramHandle mProgramHandle;
	// Attributes
	GLint mVertexPos = -1;
	// Uniforms
	GLint mPosRect = -1;
	GLint mTexture = -1;
	GLint mSrcTexelSize = -1;
	bool  mValidProgram;
};

Blitter::Impl::Impl(Graphics& graphics)
    : mGraphics { graphics }
    , mProgramHandle { graphics.NewProgram(SHADERS_FOLDER "blit.vs", SHADERS_FOLDER "blit.fs") }
    , mValidProgram { false } {
	if (mProgramHandle != nullProgram) {
		const GlProgram& program = graphics.GetProgram(mProgramHandle);
		mVertexPos = program.GetAttribLocation("inputPosition");
		mPosRect = program.GetUniformLocation("posRect");
		mTexture = program.GetUniformLocation("inputTexture");
		// mSrcTexelSize = program.GetUniformLocation("srcTexelSize");
		mValidProgram = (mVertexPos != -1 && mPosRect != -1 && mTexture != -1); // && mSrcTexelSize != -1);
	}
}

RectI Blitter::Impl::ComputeTargetRect(const GlFrameBuffer& frameBuffer) const {
	int       cx, cy, cw, ch;
	const int targetHeight = mGraphics.GetTargetWidth() * frameBuffer.GetHeight() / frameBuffer.GetWidth();
	const int targetWidth = mGraphics.GetTargetHeight() * frameBuffer.GetWidth() / frameBuffer.GetHeight();
	if (targetHeight < mGraphics.GetTargetHeight()) {
		// Center vertically
		cx = 0;
		cw = mGraphics.GetTargetWidth();
		cy = (mGraphics.GetTargetHeight() - targetHeight) / 2;
		ch = targetHeight;
	}
	else if (targetWidth < mGraphics.GetTargetWidth()) {
		// Center horizontally
		cx = (mGraphics.GetTargetWidth() - targetWidth) / 2;
		cw = targetWidth;
		cy = 0;
		ch = mGraphics.GetTargetHeight();
	}
	else {
		// Fill entire window
		cx = 0;
		cy = 0;
		cw = mGraphics.GetTargetWidth();
		ch = mGraphics.GetTargetHeight();
	}
	return { cx, cy, cx + cw, cy + ch };
}

void Blitter::Impl::Blit(const GlFrameBuffer& srcFrameBuffer) const {
	if (! mValidProgram) {
		return;
	}

	const RectI targetRect = ComputeTargetRect(srcFrameBuffer);

	PipelineState pipelineState;
	pipelineState.mDepthEnabled = false;
	pipelineState.mBlending = false;
	pipelineState.EnableScissorTest(targetRect.left, targetRect.top, targetRect.right - targetRect.left, targetRect.bottom - targetRect.top);
	PipelineHandle pipelineHandle = mGraphics.NewPipeline(pipelineState);
	mGraphics.SetPipeline(pipelineHandle);

	float x0 = (float)targetRect.left / (float)mGraphics.GetTargetWidth();
	float x1 = (float)targetRect.right / (float)mGraphics.GetTargetWidth();
	float y0 = (float)targetRect.top / (float)mGraphics.GetTargetHeight();
	float y1 = (float)targetRect.bottom / (float)mGraphics.GetTargetHeight();

	const int  uniforms[] = { mPosRect }; //, mSrcTexelSize };
	const Vec4 uniformData[] = {
		{ x0, y0, x1, y1 },
		//{ (float)srcFrameBuffer.GetWidth(), (float)srcFrameBuffer.GetHeight(), 1.f / srcFrameBuffer.GetWidth(), 1.f / srcFrameBuffer.GetHeight() },
	};
	const unsigned textureIds[] = { srcFrameBuffer.GetColorAttachment() };

	DrawCall drawCall;
	drawCall.uniforms = uniforms;
	drawCall.uniformData = uniformData;
	drawCall.numUniforms = sizeof(uniformData) / 16;
	drawCall.textures = textureIds;
	drawCall.numTextures = 1;
	drawCall.program = mProgramHandle;
	drawCall.mesh = triangleMesh;
	drawCall.drawOrder = 0;
	mGraphics.Draw(drawCall);
}

Vec2 Blitter::Impl::WindowToFrameBuffer(Vec2 winCoord, const GlFrameBuffer& frameBuffer) const {
	const RectI targetRect = ComputeTargetRect(frameBuffer);
	return {
		(winCoord.x - targetRect.left) * frameBuffer.GetWidth() / (float)(targetRect.right - targetRect.left),
		(winCoord.y - targetRect.top) * frameBuffer.GetHeight() / (float)(targetRect.bottom - targetRect.top),
	};
}

Blitter::Blitter(Graphics& graphics)
    : mPimpl { std::make_unique<Impl>(graphics) } {
}

Blitter::~Blitter() = default;

void Blitter::Blit(const GlFrameBuffer& frameBuffer) const {
	mPimpl->Blit(frameBuffer);
}

Vec2 Blitter::WindowToFrameBuffer(Vec2 winCoord, const GlFrameBuffer& frameBuffer) const {
	return mPimpl->WindowToFrameBuffer(winCoord, frameBuffer);
}

} // namespace Wind
