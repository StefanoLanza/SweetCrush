#pragma once

#include "Color.h"
#include "FwdDecl.h"
#include "Maths.h"
#include <memory>

namespace Wind {

enum class UIBlendMode {
	Off,
	On,
	Auto,
};

struct UIDrawBitmapArgs {
	Color       color = whiteColor;
	UIBlendMode blendMode = UIBlendMode::Auto;
	unsigned    priority = 0;
	float       _9patch = 0.f; // pixels
	float       grayscale = 0.f;
};

struct UITransform;

class UIRenderer final {
public:
	explicit UIRenderer(Graphics& graphics, UITextRenderer& textRenderer);
	~UIRenderer();

	const UITextRenderer& GetTextRenderer() const;
	void                  DrawBitmap(const UIRect& rect, const Texture& texture, const UIDrawBitmapArgs& args) const;
	void                  DrawSolidRect(const UIRect& rect, const Color& color, UIBlendMode blendMode) const;
	void                  DrawLine(const Vec2& start, const Vec2& end, float thickness, const Color& color, unsigned priority) const;
	void                  DrawBorder(const UIRect& rect, float thickness, const Color& color, unsigned priority) const;

private:
	class Impl;
	std::unique_ptr<Impl> mPimpl;
};

} // namespace Wind
