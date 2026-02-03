#pragma once

#include "Color.h"
#include "FwdDecl.h"
#include "Maths.h"
#include <memory>

namespace Wind {

struct UIDrawParams {
	Color    color = whiteColor;
	bool     blending = false;
	unsigned priority = 0;
	Rect     _9patch = { 0.f, 0.f, 0.f, 0.f }; // pixels
};

class UIRenderer final {
public:
	explicit UIRenderer(Graphics& graphics, TextRenderer& textRenderer);
	~UIRenderer();

	const TextRenderer& GetTextRenderer() const;
	void DrawRect(const UIRect& rect, const Texture& texture, const UIDrawParams& prms) const;
	void DrawLine(const Vec2& start, const Vec2& end, float thickness, const Color& color, unsigned priority) const;
	void DrawBorder(const UIRect& rect, float thickness, const Color& color, unsigned priority) const;

private:
	class Impl;
	std::unique_ptr<Impl> mPimpl;
};

} // namespace Wind
