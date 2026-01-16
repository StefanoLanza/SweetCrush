#include "UIRenderer.h"
#include "UI.h"
#include "Config.h"
#include "GlProgram.h"
#include "Graphics.h"
#include "Texture.h"
#include <SDL3/SDL.h>
#include <cmath>

namespace Wind {

class UIRenderer::Impl {
public:
	explicit Impl(Graphics& graphics);

	void DrawRect(const UIRect& rect, const Texture& surface, const UIDrawParams& prms) const;

private:
	Graphics&      mGraphics;
	PipelineHandle mPipelineBlending;
	ProgramHandle  mProgramHandle;
	bool           mValidPrograms;
	// Uniforms
	GLint mColor = 0;
	GLint mPosRect = 0;
	GLint mUVRect = 0;
	GLint mTexture = 0;
	GLint m9Patch = 0;
};

UIRenderer::Impl::Impl(Graphics& graphics)
    : mGraphics { graphics }
    , mProgramHandle { graphics.NewProgram(SHADERS_FOLDER "uiQuad.vs", SHADERS_FOLDER "uiQuad.fs") }
    , mValidPrograms { false } {

	PipelineState pipelineState;
	pipelineState.mBlending = true;
	mPipelineBlending = graphics.NewPipeline(pipelineState);

	if (mProgramHandle != nullProgram) {
		const GlProgram& program = graphics.GetProgram(mProgramHandle);
		mColor = program.GetUniformLocation("color");
		mPosRect = program.GetUniformLocation("posRect");
		mUVRect = program.GetUniformLocation("uvRect");
		mTexture = program.GetUniformLocation("inputTexture");
		m9Patch = program.GetUniformLocation("_9Patch");
		mValidPrograms = (mColor != -1 && mPosRect != -1 && m9Patch != -1 && mUVRect != -1 && mTexture != -1);
	}
}

void UIRenderer::Impl::DrawRect(const UIRect& rect, const Texture& texture, const UIDrawParams& prms) const {
	if (! mValidPrograms) {
		return;
	}

	const float textureWidth = static_cast<float>(texture.Width());
	const float textureHeight = static_cast<float>(texture.Height());

	const int uniforms[] = {
		mPosRect,
		mUVRect,
		mColor,
		m9Patch,
	};
	const float uniformData[][4] = {
		{ rect.pos.x, rect.pos.y, rect.width, rect.height },
		{ 0.f, 0.f, 1.f, 1.f },
		{ prms.color.r / 255.f, prms.color.g / 255.f, prms.color.b / 255.f, prms.color.a / 255.f },
		{ prms._9patch.left, prms._9patch.left / textureWidth, prms._9patch.left / textureHeight, 0.f },
	};

	if (prms.blending) {
		mGraphics.SetPipeline(mPipelineBlending);
	}
	else {
		mGraphics.SetDefaultPipeline();
	}

	const unsigned textureIds[] = { texture.GetTextureId() };

	DrawCall drawCall;
	drawCall.uniforms = uniforms;
	drawCall.uniformData = uniformData;
	drawCall.numUniforms = sizeof(uniformData) / 16;
	drawCall.textures = textureIds;
	drawCall.numTextures = 1;
	drawCall.program = mProgramHandle;
	drawCall.mesh = quadMesh;
	drawCall.drawOrder = prms.priority;
	drawCall.sortKey = (textureIds[0] & 255); // sort by texture
	mGraphics.Draw(drawCall);
}

UIRenderer::UIRenderer(Graphics& graphics)
    : mPimpl { std::make_unique<Impl>(graphics) } {
}

UIRenderer::~UIRenderer() = default;

void UIRenderer::DrawRect(const UIRect& rect, const Texture& texture, const UIDrawParams& prms) const {
	mPimpl->DrawRect(rect, texture, prms);
}

} // namespace Wind
