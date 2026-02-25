#include "Bloom.h"
#include "Config.h"
#include "GlFrameBuffer.h"
#include "GlProgram.h"
#include "Graphics.h"

namespace Wind {

class Bloom::Impl {
public:
	explicit Impl(Graphics& graphics);
	void Blit(const GlFrameBuffer& frameBuffer) const;
	Vec2 WindowToFrameBuffer(Vec2 winCoord, const GlFrameBuffer& frameBuffer) const;

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

Bloom::Impl::Impl(Graphics& graphics)
    : mGraphics { graphics }
    , mProgramHandle { graphics.NewProgram("blit.vs", "blit.fs") }
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

void Bloom::Impl::Blit(const GlFrameBuffer& srcFrameBuffer) const {
	if (! mValidProgram) {
		return;
	}

	PipelineState pipelineState;
	pipelineState.mDepthEnabled = false;
	pipelineState.mBlending = false;
	PipelineHandle pipelineHandle = mGraphics.NewPipeline(pipelineState);
	mGraphics.SetPipeline(pipelineHandle);

	const int  uniforms[] = { mPosRect }; //, mSrcTexelSize };
	const Vec4 uniformData[] = {
		0,0,0,0//{ x0, y0, x1, y1 },
		//{ (float)srcFrameBuffer.GetWidth(), (float)srcFrameBuffer.GetHeight(), 1.f / srcFrameBuffer.GetWidth(), 1.f / srcFrameBuffer.GetHeight() },
	};
	const unsigned textureIds[] = { srcFrameBuffer.GetColorAttachment() };

	DrawCall drawCall;
	drawCall.uniformLocations = uniforms;
	drawCall.uniforms = uniformData;
	drawCall.numUniforms = sizeof(uniformData) / 16;
	drawCall.textures = textureIds;
	drawCall.numTextures = 1;
	drawCall.program = mProgramHandle;
	drawCall.mesh = triangleMesh;
	drawCall.drawOrder = 0;
	mGraphics.Draw(drawCall);
}

Bloom::Bloom(Graphics& graphics)
    : mPimpl { std::make_unique<Impl>(graphics) } {
}

Bloom::~Bloom() = default;

} // namespace Wind
