#pragma once

#include "Color.h"
#include "DrawOrder.h"
#include "FwdDecl.h"
#include "Gl.h"

#include <string_view>
#include <vector>

namespace Wind {

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

enum class FontId : uint32_t {
};

class FontManager final {
public:
	FontManager();

	FontId      AddFont(const char* fontName);
	bool        IsValid(FontId fontId) const;
	const Font& GetFont(FontId fontId) const;
	const Font* TryGetFont(FontId fontId) const;

private:
	std::vector<FontPtr> mFonts;
};

class TextRenderer final {
public:
	explicit TextRenderer(Graphics& graphics);
	~TextRenderer();

	FontPtr AddFont(const char* fontName);
	void    Write(const Font& font, std::string_view text, Vec2 pos, const TextStyle& style, DrawOrderType drawOrder) const;
	void    WriteAligned(const Font& font, std::string_view text, Vec2 pos, TextAlignment horizontalAlignment, const TextStyle& style,
	                     DrawOrderType drawOrder) const;

private:
	static constexpr int maxCharsPerDraw = 256;

	Graphics&            mGraphics;
	PipelineHandle       mPipeline;
	ProgramHandle        mProgramHandle;
	std::vector<FontPtr> mFonts;
	bool                 mValidProgram;
	// Uniforms
	GLint mColor = -1;
	GLint mOutlineColor = -1;
	GLint mPosRect = -1;
	GLint mTexture = -1;
};

} // namespace Wind
