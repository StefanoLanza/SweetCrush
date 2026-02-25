#include "HalfTone.h"
#include "GlFrameBuffer.h"
#include "GlProgram.h"
#include "Graphics.h"

namespace Wind {

class HalfTone::Impl {
public:
	explicit Impl(Graphics& graphics);
	void Run(const GlFrameBuffer& src, const GlFrameBuffer& dst, float pixelSize, float radius) const;

private:
	Graphics&      mGraphics;
	ProgramHandle  mProgramHandle;
	PipelineHandle mPipeline;
	// Uniforms
	GLint mTexture = -1;
	GLint mSrcTexelSize = -1;
	GLint mGridSize = -1;
	bool  mValidProgram;
};

HalfTone::Impl::Impl(Graphics& graphics)
    : mGraphics { graphics }
    , mProgramHandle { graphics.NewProgram("postprocess/blur.vs", "postprocess/halfTone.fs") }
    , mValidProgram { false } {
	if (mProgramHandle != nullProgram) {
		const GlProgram& program = graphics.GetProgram(mProgramHandle);
		mTexture = program.GetUniformLocation("inputTexture");
		mSrcTexelSize = program.GetUniformLocation("texelSize");
		mGridSize = program.GetUniformLocation("gridSize");
		mValidProgram = (mTexture != -1 && mSrcTexelSize != -1);
	}

	PipelineState pipelineState;
	pipelineState.mDepthEnabled = false;
	pipelineState.mBlending = false;
	mPipeline = mGraphics.NewPipeline(pipelineState);
}

void HalfTone::Impl::Run(const GlFrameBuffer& src, const GlFrameBuffer& dst, float pixelSize, float radius) const {
	if (! mValidProgram) {
		return;
	}

	mGraphics.SetPipeline(mPipeline);
	mGraphics.SetFrameBuffer(dst);

	const int uniformLocs[] = {
		mSrcTexelSize,
		mGridSize,
	};
	const Vec4 uniformData[] = {
		{ 1.f / src.GetWidth(), 1.f / src.GetHeight(), (float)src.GetWidth(), (float)src.GetHeight() },
		{ pixelSize, radius, 0.f, 0.f, },
	};
	const GLuint textureIds[] = { src.GetColorAttachment() };

	const DrawCall drawCall {
		.uniformLocations = uniformLocs,
		.uniforms = uniformData,
		.numUniforms = std::size(uniformLocs),
		.textures = textureIds,
		.numTextures = 1,
		.program = mProgramHandle,
		.mesh = triangleMesh,
	};
	mGraphics.Draw(drawCall);
}

HalfTone::HalfTone(Graphics& graphics)
    : mPimpl { std::make_unique<Impl>(graphics) } {
}

HalfTone::~HalfTone() = default;

void HalfTone::Run(const GlFrameBuffer& src, const GlFrameBuffer& dst, float pixelSize, float radius) const {
	mPimpl->Run(src, dst, pixelSize, radius);
}

} // namespace Wind
