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

enum class TextDirection {
	leftToRight,
	rightToLeft,
};

struct TextStyle {
	Color color = whiteColor;
	Color outlineColor = blackColor;
	float scale = 1.f;
};

struct TextDrawArgs {
	TextAlignment horizontalAlignment = TextAlignment::left;
	TextDirection dir = TextDirection::leftToRight;
};

class TextRenderer final {
public:
	explicit TextRenderer(Graphics& graphics);
	~TextRenderer();

	void Write(const Font& font, std::string_view text, Vec2 pos, const TextStyle& style, TextDirection direction, unsigned drawOrder) const;
	void WriteAligned(const Font& font, std::string_view text, Vec2 pos, TextAlignment horizontalAlignment, TextDirection direction,
	                  const TextStyle& style, unsigned drawOrder) const;

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
