#include "Blitter.h"
#include "Config.h"
#include "GlFrameBuffer.h"
#include "GlProgram.h"
#include "Graphics.h"

namespace Wind {

class Blitter::Impl {
public:
	Impl(Graphics& graphics);
	void Blit(const GlFrameBuffer& frameBuffer) const;
	Vec2 WindowToFrameBuffer(Vec2 winCoord, const GlFrameBuffer& frameBuffer) const;

private:
	RectI ComputeTargetRect(const GlFrameBuffer& frameBuffer) const;

private:
	Graphics&     mGraphics;
	ProgramHandle mProgramHandle;
	// Attributes
	GLint mVertexPos;
	// Uniforms
	GLint mPosRect;
	GLint mUVRect;
	GLint mTexture;
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
		mUVRect = program.GetUniformLocation("uvRect");
		mTexture = program.GetUniformLocation("inputTexture");
		mValidProgram = (mVertexPos != -1 && mPosRect != -1 && mUVRect != -1 && mTexture != -1);
	}
}

RectI Blitter::Impl::ComputeTargetRect(const GlFrameBuffer& frameBuffer) const {
	int   cx, cy, cw, ch;
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
	RectI r;
	r.left = cx;
	r.right = cx + cw;
	r.bottom = cy + ch;
	r.top = cy;
	return r;
}

void Blitter::Impl::Blit(const GlFrameBuffer& frameBuffer) const {
	if (! mValidProgram) {
		return;
	}

	const RectI targetRect = ComputeTargetRect(frameBuffer);

	float u0 = (float)targetRect.left / (float)mGraphics.GetTargetWidth();
	float u1 = (float)targetRect.right / (float)mGraphics.GetTargetWidth();
	float v0 = (float)targetRect.top / (float)mGraphics.GetTargetHeight();
	float v1 = (float)targetRect.bottom / (float)mGraphics.GetTargetHeight();
	// To clip space
	u0 = -1 + 2 * u0;
	u1 = -1 + 2 * u1;
	v0 = -1 + 2 * v0;
	v1 = -1 + 2 * v1;

	const int   uniforms[] = { mPosRect, mUVRect };
	const float uniformData[] = { u0, v0, u1, v1, 0.f, 0.f, 1.f, 1.f };

	DrawCall drawCall;
	drawCall.program = mProgramHandle;
	drawCall.mesh = triangleMesh;
	drawCall.texture = frameBuffer.GetTexture();
	drawCall.drawOrder = 0;
	drawCall.blending = false;
	drawCall.uniforms = uniforms;
	drawCall.uniformData = uniformData;
	drawCall.numUniforms = 2;
	mGraphics.EnableClipRect(targetRect.left, targetRect.top, targetRect.right - targetRect.left, targetRect.bottom - targetRect.top);
	mGraphics.Draw(drawCall);
	mGraphics.DisableClipRect();
}

Vec2 Blitter::Impl::WindowToFrameBuffer(Vec2 winCoord, const GlFrameBuffer& frameBuffer) const {
	const RectI targetRect = ComputeTargetRect(frameBuffer);
	Vec2 fbCoord;
	fbCoord.x = (winCoord.x - targetRect.left) * frameBuffer.GetWidth() / (float)(targetRect.right - targetRect.left);
	fbCoord.y = (winCoord.y - targetRect.top) * frameBuffer.GetHeight() / (float)(targetRect.bottom - targetRect.top);
	return fbCoord;
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
