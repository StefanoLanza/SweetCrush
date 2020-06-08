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

void Blitter::Impl::Blit(const GlFrameBuffer& frameBuffer) const {
	if (! mValidProgram) {
		return;
	}

	float u0, u1;
	float v0, v1;
	const int targetHeight = mGraphics.GetTargetWidth() * frameBuffer.GetHeight() / frameBuffer.GetWidth();
	const int targetWidth = mGraphics.GetTargetHeight() * frameBuffer.GetWidth() / frameBuffer.GetHeight();
	if (targetHeight < mGraphics.GetTargetHeight()) {
		float y0 = (mGraphics.GetTargetHeight() - targetHeight) * 0.5f;
		float y1 = y0 + targetHeight;
		v0 = -y0 / (y1 - y0);
		v1 = v0 + mGraphics.GetTargetHeight() / (y1 - y0);
		u0 = 0.f;
		u1 = 1.f;

	}
	else if (targetWidth < mGraphics.GetTargetWidth()) {
		float x0 = (mGraphics.GetTargetWidth() - targetWidth) * 0.5f;
		float x1 = x0 + targetWidth;
		u0 = -x0 / (x1 - x0);
		u1 = u0 + mGraphics.GetTargetWidth() / (x1 - x0);
		v0 = 0.f;
		v1 = 1.f;
	}
	else {
		u0 = 0.f;
		u1 = 1.f;
		v0 = 0.f;
		v1 = 1.f;
	}


	const int   uniforms[] = { mPosRect, mUVRect };
	const float uniformData[] = { -1.f, -1.f, 1.f, 1.f, u0, v0, u1, v1 };

	DrawCall drawCall;
	drawCall.program = mProgramHandle;
	drawCall.mesh = triangleMesh;
	drawCall.texture = frameBuffer.GetTexture();
	drawCall.drawOrder = 0;
	drawCall.blending = false;
	drawCall.uniforms = uniforms;
	drawCall.uniformData = uniformData;
	drawCall.numUniforms = 2;
	mGraphics.Draw(drawCall);
}

Blitter::Blitter(Graphics& graphics)
    : mPimpl { std::make_unique<Impl>(graphics) } {
}

Blitter::~Blitter() = default;

void Blitter::Blit(const GlFrameBuffer& frameBuffer) const {
	mPimpl->Blit(frameBuffer);
}

} // namespace Wind
