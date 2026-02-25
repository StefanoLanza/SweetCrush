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
	explicit Impl(Graphics& graphics, UITextRenderer& textRenderer);

	void DrawBitmap(const UIRect& rect, const Texture& surface, const UIDrawBitmapArgs& prms) const;
	void DrawSolidRect(const UIRect& rect, const Color& color, UIBlendMode blendMode, unsigned drawOrder) const;
	void DrawLine(const Vec2& start, const Vec2& end, float thickness, const Color& color, unsigned priority) const;
	void DrawBorder(const UIRect& rect, float thickness, const Color& color, unsigned priority) const;

public:
	struct BitmapProgram {
		ProgramHandle mHandle = nullProgram;
		GLint         mColor = 0;
		GLint         mCoords = 0;
		GLint         mRotationAxis = 0;
		GLint         mTexture = 0;
		GLint         m9Patch = 0;
		bool          mValid = false;
	};
	struct LineProgram {
		ProgramHandle mHandle = nullProgram;
		GLint         mColor = 0;
		GLint         mCoords = 0;
		GLint         mThickness = 0;
		bool          mValid = false;
	};
	struct RectProgram {
		ProgramHandle mHandle = nullProgram;
		GLint         mCoords = 0;
		GLint         mColor = 0;
		bool          mValid = false;
	};
	Graphics&       mGraphics;
	UITextRenderer& mTextRenderer;
	PipelineHandle  mPipelineBlending;
	BitmapProgram   mBitmapProgram;
	RectProgram     mRectProgram;
	LineProgram     mLineProgram;
};

UIRenderer::Impl::Impl(Graphics& graphics, UITextRenderer& textRenderer)
    : mGraphics { graphics }
    , mTextRenderer { textRenderer } {

	const PipelineState pipelineState {
		.mBlending = true,
	};
	mPipelineBlending = graphics.NewPipeline(pipelineState);

	mBitmapProgram.mHandle = graphics.NewProgram("ui/uiBitmap.vs", "ui/uiBitmap.fs");
	if (mBitmapProgram.mHandle != nullProgram) {
		const GlProgram& program = graphics.GetProgram(mBitmapProgram.mHandle);
		mBitmapProgram.mColor = program.GetUniformLocation("color");
		mBitmapProgram.mCoords = program.GetUniformLocation("coords");
		mBitmapProgram.mRotationAxis = program.GetUniformLocation("rotationAxis");
		mBitmapProgram.mTexture = program.GetUniformLocation("inputTexture");
		mBitmapProgram.m9Patch = program.GetUniformLocation("_9Patch");
		mBitmapProgram.mValid = (mBitmapProgram.mColor != -1 && mBitmapProgram.mCoords != -1 && mBitmapProgram.m9Patch != -1 &&
		                         mBitmapProgram.mRotationAxis != -1 && mBitmapProgram.mTexture != -1);
	}

	mLineProgram.mHandle = graphics.NewProgram("ui/uiLine.vs", "ui/uiLine.fs");
	if (mLineProgram.mHandle != nullProgram) {
		const GlProgram& program = graphics.GetProgram(mLineProgram.mHandle);
		mLineProgram.mColor = program.GetUniformLocation("color");
		mLineProgram.mCoords = program.GetUniformLocation("coords");
		mLineProgram.mThickness = program.GetUniformLocation("thickness");
		mLineProgram.mValid = (mLineProgram.mColor != -1 && mLineProgram.mCoords != -1 && mLineProgram.mThickness != -1);
	}

	mRectProgram.mHandle = graphics.NewProgram("ui/uiRect.vs", "ui/uiRect.fs");
	if (mRectProgram.mHandle != nullProgram) {
		const GlProgram& program = graphics.GetProgram(mRectProgram.mHandle);
		mRectProgram.mCoords = program.GetUniformLocation("coords");
		mRectProgram.mColor = program.GetUniformLocation("color");
		mRectProgram.mValid = (mRectProgram.mColor != -1 && mRectProgram.mCoords != -1);
	}
}

void UIRenderer::Impl::DrawBitmap(const UIRect& rect, const Texture& texture, const UIDrawBitmapArgs& prms) const {
	if (! mBitmapProgram.mValid) {
		return;
	}

	const float textureWidth = static_cast<float>(texture.Width());
	const float textureHeight = static_cast<float>(texture.Height());

	const int uniforms[] = {
		mBitmapProgram.mCoords,
		mBitmapProgram.mRotationAxis,
		mBitmapProgram.mColor,
		mBitmapProgram.m9Patch,
	};
	const float uniformData[][4] = {
		{ rect.pos.x, rect.pos.y, rect.size.x, rect.size.y },
		{ rect.axis.x, rect.axis.y, 1.f, 1.f },
		{ prms.color.r / 255.f, prms.color.g / 255.f, prms.color.b / 255.f, prms.color.a / 255.f },
		{
		    prms._9patch,
		    prms._9patch / textureWidth,
		    prms._9patch / textureHeight,
		    prms.grayscale / 100.f,
		},
	};

	bool blending = false;
	if (prms.blendMode == UIBlendMode::On) {
		blending = true;
	}
	else if (prms.blendMode == UIBlendMode::Auto) {
		blending = (prms.color.a < 255.f) || texture.HasAlpha();
	}
	if (blending) {
		mGraphics.SetPipeline(mPipelineBlending);
	}
	else {
		mGraphics.SetDefaultPipeline();
	}

	const unsigned textureIds[] = { texture.GetGLId() };

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

void UIRenderer::Impl::DrawSolidRect(const UIRect& rect, const Color& color, UIBlendMode blendMode, unsigned drawOrder) const {
	if (! mRectProgram.mValid) {
		return;
	}

	const int uniforms[] = {
		mRectProgram.mCoords,
		mRectProgram.mColor,
	};
	const float uniformData[][4] = {
		{ rect.pos.x, rect.pos.y, rect.size.x, rect.size.y },
		{ color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f },
	};

	bool blending = false;
	if (blendMode == UIBlendMode::On) {
		blending = true;
	}
	else if (blendMode == UIBlendMode::Auto) {
		blending = (color.a < 255.f);
	}
	if (blending) {
		mGraphics.SetPipeline(mPipelineBlending);
	}
	else {
		mGraphics.SetDefaultPipeline();
	}

	const DrawCall drawCall {
		.uniformLocations = uniforms,
		.uniforms = uniformData,
		.numUniforms = std::size(uniforms),
		.program = mRectProgram.mHandle,
		.mesh = quadMesh,
		.drawOrder = drawOrder,
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

UIRenderer::UIRenderer(Graphics& graphics, UITextRenderer& textRenderer)
    : mPimpl { std::make_unique<Impl>(graphics, textRenderer) } {
}

UIRenderer::~UIRenderer() = default;

const UITextRenderer& UIRenderer::GetTextRenderer() const {
	return mPimpl->mTextRenderer;
}

void UIRenderer::DrawBitmap(const UIRect& rect, const Texture& texture, const UIDrawBitmapArgs& prms) const {
	mPimpl->DrawBitmap(rect, texture, prms);
}

void UIRenderer::DrawSolidRect(const UIRect& rect, const Color& color, UIBlendMode blendMode, unsigned drawOrder) const {
	mPimpl->DrawSolidRect(rect, color, blendMode, drawOrder);
}

void UIRenderer::DrawLine(const Vec2& start, const Vec2& end, float thickness, const Color& color, unsigned drawOrder) const {
	if (color.a <= 0.f) {
		return;
	}
	mPimpl->DrawLine(start, end, thickness, color, drawOrder);
}

void UIRenderer::DrawBorder(const UIRect& rect, float thickness, const Color& color, unsigned drawOrder) const {
	if (color.a <= 0.f) {
		return;
	}
	mPimpl->DrawBorder(rect, thickness, color, drawOrder);
}

} // namespace Wind
