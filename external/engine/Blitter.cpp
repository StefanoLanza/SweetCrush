#include "Blitter.h"
#include "Config.h"
#include "GlFrameBuffer.h"
#include "GlProgram.h"
#include "Graphics.h"

namespace Wind {

class Blitter::Impl {
public:
	explicit Impl(Graphics& graphics);
	void Blit(const GlFrameBuffer& frameBuffer, BlitFilter filter) const;
	Vec2 WindowToFrameBuffer(Vec2 winCoord, const GlFrameBuffer& frameBuffer) const;

private:
	RectI ComputeTargetRect(const GlFrameBuffer& srcFrameBuffer) const;
	struct BlitProgram;
	void InitProgram(BlitProgram& program, Graphics& graphics, const char* fs) const;

private:
	struct BlitProgram {
		ProgramHandle mHandle = nullProgram;
		// Uniforms
		GLint mPosRect = -1;
		GLint mTexture = -1;
		GLint mSrcTexelSize = -1;
		bool  mValid;
	};

	BlitProgram mPrograms[2];
	Graphics&   mGraphics;
	GLuint      mSamplers[2];
};

Blitter::Impl::Impl(Graphics& graphics)
    : mGraphics { graphics } {
	InitProgram(mPrograms[0], graphics, SHADERS_FOLDER "blit.fs");
	InitProgram(mPrograms[1], graphics, SHADERS_FOLDER "blitCubic.fs");

	glGenSamplers(2, mSamplers);
	// Sampler 0: Point (Nearest) Filtering
	glSamplerParameteri(mSamplers[0], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glSamplerParameteri(mSamplers[0], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glSamplerParameteri(mSamplers[0], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(mSamplers[0], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// Sampler 1: Linear Filtering
	glSamplerParameteri(mSamplers[1], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameteri(mSamplers[1], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(mSamplers[1], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(mSamplers[1], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void Blitter::Impl::InitProgram(BlitProgram& blitProgram, Graphics& graphics, const char* fs) const {
	blitProgram.mHandle = graphics.NewProgram(SHADERS_FOLDER "blit.vs", SHADERS_FOLDER "blit.fs");

	if (blitProgram.mHandle != nullProgram) {
		const GlProgram& program = graphics.GetProgram(blitProgram.mHandle);
		blitProgram.mPosRect = program.GetUniformLocation("posRect");
		blitProgram.mTexture = program.GetUniformLocation("inputTexture");
		blitProgram.mSrcTexelSize = program.GetUniformLocation("srcTexelSize");
		blitProgram.mValid = (blitProgram.mPosRect != -1 && blitProgram.mTexture != -1); // && mSrcTexelSize != -1);
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

void Blitter::Impl::Blit(const GlFrameBuffer& srcFrameBuffer, BlitFilter filter) const {
	const BlitProgram& program = mPrograms[0];
	if (! program.mValid) {
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

	const int  uniforms[] = { program.mPosRect }; //, mSrcTexelSize };
	const Vec4 uniformData[] = {
		{ x0, y0, x1, y1 },
		//{ (float)srcFrameBuffer.GetWidth(), (float)srcFrameBuffer.GetHeight(), 1.f / srcFrameBuffer.GetWidth(), 1.f / srcFrameBuffer.GetHeight() },
	};
	const unsigned textureIds[] = { srcFrameBuffer.GetColorAttachment() };
	const unsigned samplers[] = { mSamplers[0] };

	DrawCall drawCall;
	drawCall.uniforms = uniforms;
	drawCall.uniformData = uniformData;
	drawCall.numUniforms = sizeof(uniformData) / 16;
	drawCall.textures = textureIds;
	drawCall.samplers = samplers;
	drawCall.numTextures = 1;
	drawCall.program = program.mHandle;
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

void Blitter::Blit(const GlFrameBuffer& srcFrameBuffer, BlitFilter filter) const {
	mPimpl->Blit(srcFrameBuffer, filter);
}

Vec2 Blitter::WindowToFrameBuffer(Vec2 winCoord, const GlFrameBuffer& frameBuffer) const {
	return mPimpl->WindowToFrameBuffer(winCoord, frameBuffer);
}

} // namespace Wind
