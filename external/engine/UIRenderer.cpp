#include "UIRenderer.h"
#include "Config.h"
#include "GlProgram.h"
#include "Graphics.h"
#include "Texture.h"
#include "UI.h"
#include <SDL3/SDL.h>
#include <cmath>

namespace Wind {

class UIRenderer::Impl {
public:
	explicit Impl(Graphics& graphics, TextRenderer& textRenderer);

	void DrawRect(const UIRect& rect, const Texture& surface, const UIDrawParams& prms) const;
	void DrawLine(const Vec2& start, const Vec2& end, float thickness, const Color& color, unsigned priority) const;
	void DrawBorder(const UIRect& rect, float thickness, const Color& color, unsigned priority) const;

public:
	struct BitmapProgram {
		ProgramHandle mHandle;
		GLint         mColor = 0;
		GLint         mPosRect = 0;
		GLint         mUVRect = 0;
		GLint         mTexture = 0;
		GLint         m9Patch = 0;
		bool          mValid = false;
	};
	struct LineProgram {
		ProgramHandle mHandle;
		GLint         mColor = 0;
		GLint         mCoords = 0;
		GLint         mThickness = 0;
		bool          mValid = false;
	};
	Graphics&      mGraphics;
	TextRenderer&  mTextRenderer;
	PipelineHandle mPipelineBlending;
	BitmapProgram  mBitmapProgram;
	LineProgram    mLineProgram;
};

UIRenderer::Impl::Impl(Graphics& graphics, TextRenderer& textRenderer)
    : mGraphics { graphics }
    , mTextRenderer { textRenderer } {

	const PipelineState pipelineState {
		.mBlending = true,
	};
	mPipelineBlending = graphics.NewPipeline(pipelineState);

	mBitmapProgram.mHandle = graphics.NewProgram(SHADERS_FOLDER "uiQuad.vs", SHADERS_FOLDER "uiQuad.fs");
	if (mBitmapProgram.mHandle != nullProgram) {
		const GlProgram& program = graphics.GetProgram(mBitmapProgram.mHandle);
		mBitmapProgram.mColor = program.GetUniformLocation("color");
		mBitmapProgram.mPosRect = program.GetUniformLocation("posRect");
		mBitmapProgram.mUVRect = program.GetUniformLocation("uvRect");
		mBitmapProgram.mTexture = program.GetUniformLocation("inputTexture");
		mBitmapProgram.m9Patch = program.GetUniformLocation("_9Patch");
		mBitmapProgram.mValid = (mBitmapProgram.mColor != -1 && mBitmapProgram.mPosRect != -1 && mBitmapProgram.m9Patch != -1 &&
		                         mBitmapProgram.mUVRect != -1 && mBitmapProgram.mTexture != -1);
	}

	mLineProgram.mHandle = graphics.NewProgram(SHADERS_FOLDER "uiLine.vs", SHADERS_FOLDER "uiLine.fs");
	if (mLineProgram.mHandle != nullProgram) {
		const GlProgram& program = graphics.GetProgram(mLineProgram.mHandle);
		mLineProgram.mColor = program.GetUniformLocation("color");
		mLineProgram.mCoords = program.GetUniformLocation("coords");
		mLineProgram.mThickness = program.GetUniformLocation("thickness");
		mLineProgram.mValid = (mLineProgram.mColor != -1 && mLineProgram.mCoords != -1 && mLineProgram.mThickness != -1);
	}
}

void UIRenderer::Impl::DrawRect(const UIRect& rect, const Texture& texture, const UIDrawParams& prms) const {
	if (! mBitmapProgram.mValid) {
		return;
	}

	const float textureWidth = static_cast<float>(texture.Width());
	const float textureHeight = static_cast<float>(texture.Height());

	const int uniforms[] = {
		mBitmapProgram.mPosRect,
		mBitmapProgram.mUVRect,
		mBitmapProgram.mColor,
		mBitmapProgram.m9Patch,
	};
	const float uniformData[][4] = {
		{ rect.pos.x, rect.pos.y, rect.size.x, rect.size.y },
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

	const DrawCall drawCall {
		.uniformLocations = uniforms,
		.uniforms = uniformData,
		.numUniforms = std::size(uniforms),
		.textures = textureIds,
		.numTextures = 1,
		.program = mBitmapProgram.mHandle,
		.mesh = quadMesh,
		.drawOrder = prms.priority,
		.sortKey = (textureIds[0] & 255), // sort by texture
	};
	mGraphics.Draw(drawCall);
}

void UIRenderer::Impl::DrawLine(const Vec2& start, const Vec2& end, float thickness, const Color& color, unsigned priority) const {
	if (! mLineProgram.mValid) {
		return;
	}

	const int uniforms[] = {
		mLineProgram.mCoords,
		mLineProgram.mColor,
		mLineProgram.mThickness,
	};
	const float uniformData[][4] = {
		{ start.x, start.y, end.x, end.y },
		{ color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f },
		{ thickness, 0.f, 0.f, 0.f },
	};

	mGraphics.SetPipeline(mPipelineBlending);

	const DrawCall drawCall {
		.uniformLocations = uniforms,
		.uniforms = uniformData,
		.numUniforms = std::size(uniforms),
		.program = mLineProgram.mHandle,
		.mesh = quadMesh,
		.drawOrder = priority,
	};
	mGraphics.Draw(drawCall);
}

void UIRenderer::Impl::DrawBorder(const UIRect& rect, float thickness, const Color& color, unsigned priority) const {
}

UIRenderer::UIRenderer(Graphics& graphics, TextRenderer& textRenderer)
    : mPimpl { std::make_unique<Impl>(graphics, textRenderer) } {
}

UIRenderer::~UIRenderer() = default;

const TextRenderer& UIRenderer::GetTextRenderer() const {
	return mPimpl->mTextRenderer;
}

void UIRenderer::DrawRect(const UIRect& rect, const Texture& texture, const UIDrawParams& prms) const {
	mPimpl->DrawRect(rect, texture, prms);
}

void UIRenderer::DrawLine(const Vec2& start, const Vec2& end, float thickness, const Color& color, unsigned priority) const {
	mPimpl->DrawLine(start, end, thickness, color, priority);
}

void UIRenderer::DrawBorder(const UIRect& rect, float thickness, const Color& color, unsigned priority) const {
	mPimpl->DrawBorder(rect, thickness, color, priority);
}

} // namespace Wind
