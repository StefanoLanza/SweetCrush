#include "BitmapRender.h"
#include "Config.h"
#include "GlProgram.h"
#include "Graphics.h"
#include "SdlSurface.h"
#include <SDL.h>
#include <cmath>

namespace Wind {

class BitmapRenderer::Impl {
public:
	Impl(Graphics& graphics);

	void DrawBitmap(const SdlSurface& bitmap, float x, float y) const;
	void DrawBitmapEx(const SdlSurface& surface, float x, float y, const BitmapExtParams& prm) const;

private:
	Graphics&     mGraphics;
	ProgramHandle mProgramHandle;
	bool          mValidProgram;
	// Uniforms
	GLint mColor;
	GLint mRotation;
	GLint mPosRect;
	GLint mUVRect;
	GLint mTexture;
};

BitmapRenderer::Impl::Impl(Graphics& graphics)
    : mGraphics { graphics }
    , mProgramHandle { graphics.NewProgram(SHADERS_FOLDER "quad.vs", SHADERS_FOLDER "quad.fs") }
    , mValidProgram { false } {
	if (mProgramHandle != nullProgram) {
		const GlProgram& program = graphics.GetProgram(mProgramHandle);
		mColor = program.GetUniformLocation("color");
		mRotation = program.GetUniformLocation("rotation");
		mPosRect = program.GetUniformLocation("posRect");
		mUVRect = program.GetUniformLocation("uvRect");
		mTexture = program.GetUniformLocation("inputTexture");
		mValidProgram = (mColor != -1 && mPosRect != -1 && mUVRect != -1 && mTexture != -1);
		;
	}
}

void BitmapRenderer::Impl::DrawBitmap(const SdlSurface& bitmap, float x, float y) const {
	DrawBitmapEx(bitmap, x, y, BitmapExtParams {});
}

void BitmapRenderer::Impl::DrawBitmapEx(const SdlSurface& bitmap, float x, float y, const BitmapExtParams& prm) const {
	if (! mValidProgram) {
		return;
	}

	const float bitmapWidth = (prm.width <= 0.f ? static_cast<float>(bitmap.Width()) : prm.width) * prm.scale;
	const float bitmapHeight = (prm.height <= 0.f ? static_cast<float>(bitmap.Height()) : prm.height) * prm.scale;
	const float pivot_x = bitmapWidth * prm.pivot.x;
	const float pivot_y = bitmapHeight * prm.pivot.y;

	float left = x - pivot_x;
	float top = y - pivot_y;
	float right = left + bitmapWidth;
	float bottom = top + bitmapHeight;

	const int   uniforms[] = { mPosRect, mUVRect, mColor, mRotation };
	const float uniformData[][4] = {
		{ left, top, right, bottom },
		{ prm.texRect.left, prm.texRect.top, prm.texRect.right, prm.texRect.bottom },
		{ prm.color.r / 255.f, prm.color.g / 255.f, prm.color.b / 255.f, prm.color.a / 255.f },
		{ std::cos(prm.orientation), std::sin(prm.orientation), x, y },
	};

	DrawCall drawCall;
	drawCall.program = mProgramHandle;
	drawCall.mesh = quadMesh;
	drawCall.texture = bitmap.GetTextureId();
	drawCall.drawOrder = prm.drawOrder;
	drawCall.blending = prm.blending;
	drawCall.uniforms = uniforms;
	drawCall.uniformData = reinterpret_cast<const float*>(uniformData);
	drawCall.numUniforms = 4;
	mGraphics.Draw(drawCall);
}

BitmapRenderer::BitmapRenderer(Graphics& graphics)
    : mPimpl { std::make_unique<Impl>(graphics) } {
}

BitmapRenderer::~BitmapRenderer() = default;

void BitmapRenderer::DrawBitmap(const SdlSurface& bitmap, Vec2 pos) const {
	mPimpl->DrawBitmap(bitmap, pos.x, pos.y);
}

void BitmapRenderer::DrawBitmapEx(const SdlSurface& surface, Vec2 pos, const BitmapExtParams& prm) const {
	mPimpl->DrawBitmapEx(surface, pos.x, pos.y, prm);
}

} // namespace Wind
