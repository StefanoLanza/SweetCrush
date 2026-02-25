#include "Blur.h"
#include "Config.h"
#include "GlFrameBuffer.h"
#include "GlProgram.h"
#include "Graphics.h"

namespace Wind {

class Blur::Impl {
public:
	explicit Impl(Graphics& graphics);
	void Run(const GlFrameBuffer& src, const GlFrameBuffer& dst, const GlFrameBuffer& temp) const;

private:
	void RunPass(const GlFrameBuffer& src, bool hrz) const;

private:
	Graphics&     mGraphics;
	ProgramHandle mProgramHandle;
	// Uniforms
	GLint mTexture = -1;
	GLint mSrcTexelSize = -1;
	bool  mValidProgram;
};

Blur::Impl::Impl(Graphics& graphics)
    : mGraphics { graphics }
    , mProgramHandle { graphics.NewProgram("postprocess/blur.vs", "postprocess/blur.fs") }
    , mValidProgram { false } {
	if (mProgramHandle != nullProgram) {
		const GlProgram& program = graphics.GetProgram(mProgramHandle);
		mTexture = program.GetUniformLocation("inputTexture");
		mSrcTexelSize = program.GetUniformLocation("texelSize");
		mValidProgram = (mTexture != -1 && mSrcTexelSize != -1);
	}
}

void Blur::Impl::RunPass(const GlFrameBuffer& src, bool hrz) const {
	Vec2       srcSize { hrz ? (float)1.f / src.GetWidth() : 0.f, hrz ? 0.f : 1.f / src.GetHeight() };
	const int  uniforms[] = { mSrcTexelSize };
	const Vec4 uniformData[] = {
		{ srcSize.x, srcSize.y, 0.f, 0.f },
	};
	const unsigned textureIds[] = { src.GetColorAttachment() };

	DrawCall drawCall {
		.uniformLocations = uniforms,
		.uniforms = uniformData,
		.numUniforms = std::size(uniforms),
		.textures = textureIds,
		.numTextures = 1,
		.program = mProgramHandle,
		.mesh = triangleMesh,
	};
	mGraphics.Draw(drawCall);
}

void Blur::Impl::Run(const GlFrameBuffer& src, const GlFrameBuffer& dst, const GlFrameBuffer& temp) const {
	if (! mValidProgram) {
		return;
	}

	PipelineState pipelineState;
	pipelineState.mDepthEnabled = false;
	pipelineState.mBlending = false;
	PipelineHandle pipelineHandle = mGraphics.NewPipeline(pipelineState);
	mGraphics.SetPipeline(pipelineHandle);

	mGraphics.SetFrameBuffer(temp);
	RunPass(src, true);

	mGraphics.SetFrameBuffer(dst);
	RunPass(temp, false);
}

Blur::Blur(Graphics& graphics)
    : mPimpl { std::make_unique<Impl>(graphics) } {
}

Blur::~Blur() = default;

void Blur::Run(const GlFrameBuffer& src, const GlFrameBuffer& dst, const GlFrameBuffer& temp) const {
	mPimpl->Run(src, dst, temp);
}

} // namespace Wind
