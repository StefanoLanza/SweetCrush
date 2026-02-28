#include "BitmapRender.h"
#include "Config.h"
#include "GlProgram.h"
#include "Graphics.h"
#include "Texture.h"
#include <SDL3/SDL.h>
#include <cmath>

namespace Wind {

class BitmapRenderer::Impl {
public:
	explicit Impl(Graphics& graphics);

	void DrawBitmap(const Texture& bitmap, float x, float y) const;
	void DrawBitmapEx(const Texture& surface, float x, float y, const BitmapExtParams& prm) const;

private:
	Graphics&      mGraphics;
	PipelineHandle mPipelineBlending;
	ProgramHandle  mProgramHandle;
	bool           mValidPrograms;
	// Uniforms
	GLint mColor = 0;
	GLint mRotation = 0;
	GLint mCoords = 0;
	GLint mUVRect = 0;
	GLint mMisc = 0;
	GLint mTexture = 0;
};

BitmapRenderer::Impl::Impl(Graphics& graphics)
    : mGraphics { graphics }
    , mProgramHandle { graphics.NewProgram("bitmap.vs", "bitmap.fs") }
    , mValidPrograms { false } {

	PipelineState pipelineState;
	pipelineState.blending = true;
	mPipelineBlending = graphics.NewPipeline(pipelineState);

	if (mProgramHandle != nullProgram) {
		const GlProgram& program = graphics.GetProgram(mProgramHandle);
		mColor = program.GetUniformLocation("color");
		mRotation = program.GetUniformLocation("rotation");
		mCoords = program.GetUniformLocation("posRect");
		mUVRect = program.GetUniformLocation("uvRect");
		mMisc = program.GetUniformLocation("misc");
		mTexture = program.GetUniformLocation("inputTexture");
		mValidPrograms = (mColor != -1 && mCoords != -1 && mUVRect != -1 && mMisc != -1 && mTexture != -1);
	}
}

void BitmapRenderer::Impl::DrawBitmap(const Texture& bitmap, float x, float y) const {
	DrawBitmapEx(bitmap, x, y, BitmapExtParams {});
}

void BitmapRenderer::Impl::DrawBitmapEx(const Texture& bitmap, float x, float y, const BitmapExtParams& prm) const {
	if (! mValidPrograms) {
		return;
	}

	const float bitmapWidth = static_cast<float>(bitmap.Width());
	const float bitmapHeight = static_cast<float>(bitmap.Height());
	const float rectWidth = (prm.width <= 0.f ? bitmapWidth : prm.width) * prm.scale.x;
	const float rectHeight = (prm.height <= 0.f ? bitmapHeight : prm.height) * prm.scale.y;
	const float pivot_x = rectWidth * prm.pivot.x;
	const float pivot_y = rectHeight * prm.pivot.y;

	const int uniforms[] = {
		mCoords, mUVRect, mColor, mRotation, mMisc,
	};
	const float uniformData[][4] = {
		{ x - pivot_x, y - pivot_y, rectWidth, rectHeight },
		{ prm.texRect.left, prm.texRect.top, prm.texRect.right, prm.texRect.bottom },
		{ prm.color.r / 255.f, prm.color.g / 255.f, prm.color.b / 255.f, prm.color.a / 255.f },
		{ std::cos(prm.orientation), std::sin(prm.orientation), x, y },
		{ prm.grayscale / 100.f, 0.f, 0.f, 0.f },
	};

	if (prm.blending) {
		mGraphics.SetPipeline(mPipelineBlending);
	}
	else {
		mGraphics.SetDefaultPipeline();
	}

	const unsigned textureIds[] = { bitmap.GetGLId() };

	DrawCall drawCall;
	drawCall.uniformLocations = uniforms;
	drawCall.uniforms = uniformData;
	drawCall.numUniforms = std::size(uniforms);
	drawCall.textures = textureIds;
	drawCall.numTextures = 1;
	drawCall.program = mProgramHandle;
	drawCall.mesh = quadMesh;
	drawCall.drawOrder = prm.drawOrder;
	drawCall.sortKey = (bitmap.GetGLId() & 255); // sort by texture
	mGraphics.Draw(drawCall);
}

BitmapRenderer::BitmapRenderer(Graphics& graphics)
    : mPimpl { std::make_unique<Impl>(graphics) } {
}

BitmapRenderer::~BitmapRenderer() = default;

void BitmapRenderer::DrawBitmap(const Texture& bitmap, Vec2 pos) const {
	mPimpl->DrawBitmap(bitmap, pos.x, pos.y);
}

void BitmapRenderer::DrawBitmapEx(const Texture& surface, Vec2 pos, const BitmapExtParams& prm) const {
	mPimpl->DrawBitmapEx(surface, pos.x, pos.y, prm);
}

} // namespace Wind
