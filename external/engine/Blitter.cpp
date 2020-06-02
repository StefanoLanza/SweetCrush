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

	const int   uniforms[] = { mPosRect, mUVRect };
	const float uniformData[] = { -1.f, -1.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f };

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
