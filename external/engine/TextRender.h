#pragma once

#include "Color.h"
#include "DrawOrder.h"
#include "FwdDecl.h"
#include "Gl.h"
#include <memory>
#include <vector>

namespace Wind {

class SdlSurface;
class SdlWindow;
class Graphics;

enum class TextAlignment {
	left,
	right,
	center,
};

struct TextStyle {
	Color color;
	Color outlineColor;
};

constexpr TextStyle defaultTextStyle = {
	whiteColor,
	blackColor,
};

class TextRenderer {
public:
	TextRenderer(Graphics& graphics);
	~TextRenderer();

	FontPtr AddFont(const char* fontName);
	void    Write(const Font& font, const char* text, Vec2 pos, const TextStyle& style, DrawOrderType drawOrder) const;
	void    WriteAligned(const Font& font, const char* text, Vec2 pos, TextAlignment horizontalAlignment, const TextStyle& style,
	                     DrawOrderType drawOrder) const;

private:
	Graphics&            mGraphics;
	ProgramHandle        mProgramHandle;
	std::vector<FontPtr> mFonts;
	// Uniforms
	GLint mColor;
	GLint mOutlineColor;
	GLint mPosRect;
	GLint mRotation;
	GLint mUVRect;
	GLint mTexture;
};

} // namespace Wind
