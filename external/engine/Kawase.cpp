#include "Kawase.h"
#include "Config.h"
#include "GlFrameBuffer.h"
#include "GlProgram.h"
#include "Graphics.h"

namespace Wind {

class KawaseBlur::Impl {
public:
	explicit Impl(Graphics& graphics);
	void Run(const GlFrameBuffer& src, const GlFrameBuffer* mipChain[], int numPasses) const;

private:
	void Downsample(const GlFrameBuffer& src, const GlFrameBuffer& dst) const;
	void Upsample(const GlFrameBuffer& src, const GlFrameBuffer& dst) const;

private:
	Graphics&     mGraphics;
	ProgramHandle mDownsampleProgram;
	ProgramHandle mUpsampleProgram;
	// Uniforms
	GLint mTexture = -1;
	GLint mSrcTexelSize = -1;
	bool  mValidPrograms;
};

KawaseBlur::Impl::Impl(Graphics& graphics)
    : mGraphics { graphics }
    , mDownsampleProgram { graphics.NewProgram(SHADERS_FOLDER "fullscreenTriangle.vs", SHADERS_FOLDER "postprocess/kawaseDownsample.fs") }
    , mUpsampleProgram { graphics.NewProgram(SHADERS_FOLDER "fullscreenTriangle.vs", SHADERS_FOLDER "postprocess/kawaseUpsample.fs") }
    , mValidPrograms { false } {
	if (mDownsampleProgram != nullProgram) {
		const GlProgram& program = graphics.GetProgram(mDownsampleProgram);
		mTexture = program.GetUniformLocation("sourceTexture");
		mSrcTexelSize = program.GetUniformLocation("texelSize");
		mValidPrograms = (mTexture != -1 && mSrcTexelSize != -1);
	}
	if (mUpsampleProgram == nullProgram) {
		mValidPrograms = false;
	}
}

void KawaseBlur::Impl::Downsample(const GlFrameBuffer& src, const GlFrameBuffer& dst) const {
	mGraphics.SetFrameBuffer(dst);

	const int  uniforms[] = { mSrcTexelSize };
	const Vec4 uniformData[] = {
		{ (float)1.f / src.GetWidth(), 1.f / src.GetHeight(), 0.f, 0.f },
	};
	const unsigned textureIds[] = { src.GetColorAttachment() };

	const DrawCall drawCall {
		.uniformLocations = uniforms,
		.uniforms = uniformData,
		.numUniforms = std::size(uniforms),
		.textures = textureIds,
		.numTextures = 1,
		.program = mDownsampleProgram,
		.mesh = triangleMesh,
	};
	mGraphics.Draw(drawCall);
}

void KawaseBlur::Impl::Upsample(const GlFrameBuffer& src, const GlFrameBuffer& dst) const {
	mGraphics.SetFrameBuffer(dst);

	const int  uniforms[] = { mSrcTexelSize };
	const Vec4 uniformData[] = {
		{ (float)1.f / src.GetWidth(), 1.f / src.GetHeight(), 0.f, 0.f },
	};
	const unsigned textureIds[] = { src.GetColorAttachment() };

	const DrawCall drawCall {
		.uniformLocations = uniforms,
		.uniforms = uniformData,
		.numUniforms = std::size(uniforms),
		.textures = textureIds,
		.numTextures = 1,
		.program = mUpsampleProgram,
		.mesh = triangleMesh,
	};
	mGraphics.Draw(drawCall);
}

void KawaseBlur::Impl::Run(const GlFrameBuffer& src, const GlFrameBuffer* mipChain[], int numPasses) const {
	if (! mValidPrograms) {
		return;
	}

	PipelineState pipelineState;
	pipelineState.mDepthEnabled = false;
	pipelineState.mBlending = false;
	PipelineHandle pipelineHandle = mGraphics.NewPipeline(pipelineState);
	mGraphics.SetPipeline(pipelineHandle);

	for (int i = 0; i < numPasses; ++i) {
		Downsample(i == 0 ? src : *mipChain[i - 1], *mipChain[i]);
	}

	for (int i = numPasses - 1; i > 0; --i) {
		Upsample(*mipChain[i], *mipChain[i - 1]);
	}
}

KawaseBlur::KawaseBlur(Graphics& graphics)
    : mPimpl { std::make_unique<Impl>(graphics) } {
}

KawaseBlur::~KawaseBlur() = default;

void KawaseBlur::Run(const GlFrameBuffer& src, const GlFrameBuffer* mipChain[], int numPasses) const {
	mPimpl->Run(src, mipChain, numPasses);
}

} // namespace Wind
