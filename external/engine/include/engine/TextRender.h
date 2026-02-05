#pragma once

#include "Color.h"
#include "FwdDecl.h"
#include "Gl.h"

#include <string_view>

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

class TextRenderer final {
public:
	explicit TextRenderer(Graphics& graphics);
	~TextRenderer();

	void Write(const Font& font, std::string_view text, Vec2 pos, const TextStyle& style, unsigned drawOrder) const;
	void WriteAligned(const Font& font, std::string_view text, Vec2 pos, TextAlignment horizontalAlignment, const TextStyle& style,
	                  unsigned drawOrder) const;

private:
	static constexpr int maxCharsPerDraw = 256;

	Graphics&      mGraphics;
	PipelineHandle mPipeline;
	ProgramHandle  mProgramHandle;
	bool           mValidProgram;
	// Uniforms
	GLint mColor = -1;
	GLint mOutlineColor = -1;
	GLint mPosRect = -1;
	GLint mTexture = -1;
};

} // namespace Wind
