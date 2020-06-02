#pragma once

#include "Color.h"
#include "DrawOrder.h"
#include "FwdDecl.h"
#include "Gl.h"
#include "Math.h"
#include <memory>

namespace Wind {

struct BitmapPivot {
	static constexpr Vec2 topLeft { 0.f, 0.f };
	static constexpr Vec2 center { 0.5f, 0.5f };
};

constexpr Rect defaultTexCoord = { 0.f, 0.f, 1.f, 1.f };

struct BitmapExtParams {
	float         width = 0.f;
	float         height = 0.f;
	float         scale = 1.f;
	float         orientation = 0.f;
	Vec2          pivot = { 0.f, 0.f };
	Rect          texRect = defaultTexCoord;
	Color         color = whiteColor;
	DrawOrderType drawOrder = 0;
	bool          blending = true;
};

class BitmapRenderer {
public:
	BitmapRenderer(Graphics& graphics);
	~BitmapRenderer();

	void DrawBitmap(const SdlSurface& surface, Vec2 pos) const;
	void DrawBitmapEx(const SdlSurface& surface, Vec2 pos, const BitmapExtParams& prm) const;

private:
	class Impl;
	std::unique_ptr<Impl> mPimpl;
};

} // namespace Wind
