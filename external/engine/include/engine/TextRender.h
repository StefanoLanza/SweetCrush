#pragma once

#include "Color.h"
#include "FwdDecl.h"
#include "Gl.h"

#include <string_view>

namespace Wind {

struct UITextStyle {
	Color color = whiteColor;
	Color outlineColor = blackColor;
	Color shadowColor = blackColor;
	bool  shadow = false;
	Vec2  shadowOffset = { 0.f, 4.f };
};

struct UIRect;

class UITextRenderer final {
public:
	explicit UITextRenderer(Graphics& graphics);
	~UITextRenderer();

	void Write(const Font& font, std::string_view text, const UIRect& rect, const UITextStyle& style, unsigned drawOrder) const;
	void Write(const Font& font, std::string_view text, Vec2 pos, const UITextStyle& style, unsigned drawOrder) const;

private:

	void WriteImpl(const Font& font, std::string_view text, Vec2 pos, Vec2 scale, const UITextStyle& style, unsigned drawOrder) const;

private:
	static constexpr int maxCharsPerDraw = 256;

	Graphics&      mGraphics;
	PipelineHandle mPipeline;
	ProgramHandle  mProgramHandle;
	bool           mValidProgram;
	GLuint         mSampler;
	// Uniforms
	GLint mColor = -1;
	GLint mOutlineColor = -1;
	GLint mCoords = -1;
	GLint mPosOffset = -1;
	GLint mTexture = -1;
};

} // namespace Wind
