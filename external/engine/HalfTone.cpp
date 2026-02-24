#include "HalfTone.h"
#include "Config.h"
#include "GlFrameBuffer.h"
#include "GlProgram.h"
#include "Graphics.h"

namespace Wind {

class HalfTone::Impl {
public:
	explicit Impl(Graphics& graphics);
	void Run(const GlFrameBuffer& src, const GlFrameBuffer& dst) const;

private:
	Graphics&     mGraphics;
	ProgramHandle mProgramHandle;
	// Uniforms
	GLint mTexture = -1;
	GLint mSrcTexelSize = -1;
	bool  mValidProgram;
};

HalfTone::Impl::Impl(Graphics& graphics)
    : mGraphics { graphics }
    , mProgramHandle { graphics.NewProgram(SHADERS_FOLDER "blur.vs", SHADERS_FOLDER "halfTone.fs") }
    , mValidProgram { false } {
	if (mProgramHandle != nullProgram) {
		const GlProgram& program = graphics.GetProgram(mProgramHandle);
		mTexture = program.GetUniformLocation("inputTexture");
		mSrcTexelSize = program.GetUniformLocation("texelSize");
		mValidProgram = (mTexture != -1 && mSrcTexelSize != -1);
	}
}

void HalfTone::Impl::Run(const GlFrameBuffer& src, const GlFrameBuffer& dst) const {
	if (! mValidProgram) {
		return;
	}

	PipelineState pipelineState;
	pipelineState.mDepthEnabled = false;
	pipelineState.mBlending = false;
	PipelineHandle pipelineHandle = mGraphics.NewPipeline(pipelineState);
	mGraphics.SetPipeline(pipelineHandle);

	mGraphics.SetFrameBuffer(dst);
	Vec2       srcSize { (float)1.f / src.GetWidth(), 1.f / src.GetHeight() };
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

HalfTone::HalfTone(Graphics& graphics)
    : mPimpl { std::make_unique<Impl>(graphics) } {
}

HalfTone::~HalfTone() = default;

void HalfTone::Run(const GlFrameBuffer& src, const GlFrameBuffer& dst) const {
	mPimpl->Run(src, dst);
}

} // namespace Wind
