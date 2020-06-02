#include "TextRender.h"
#include "Config.h"
#include "DrawOrder.h"
#include "Font.h"
#include "Gl.h"
#include "GlProgram.h"
#include "Graphics.h"
#include "SdlSurface.h"
#include "SdlWindow.h"
#include <SDL.h>
#include <cassert>

namespace Wind {

TextRenderer::TextRenderer(Graphics& graphics)
    : mGraphics { graphics }
    , mProgramHandle { graphics.NewProgram(SHADERS_FOLDER "quad.vs", SHADERS_FOLDER "font.fs") } {
	const GlProgram& program = graphics.GetProgram(mProgramHandle);
	mColor = program.GetUniformLocation("color");
	mOutlineColor = program.GetUniformLocation("outlineColor");
	mPosRect = program.GetUniformLocation("posRect");
	mRotation = program.GetUniformLocation("rotation");
	mUVRect = program.GetUniformLocation("uvRect");
	mTexture = program.GetUniformLocation("inputTexture");
}

TextRenderer::~TextRenderer() = default;

FontPtr TextRenderer::AddFont(const char* fontName) {
	try {
		for (const FontPtr& font : mFonts) {
			if (font->GetName() == fontName) {
				return font;
			}
		}

		char textureFile[260];
		snprintf(textureFile, sizeof(textureFile), "%s_0.png", fontName);

		char texturePath[260];
		snprintf(texturePath, sizeof(texturePath), "%s%s_0.png", FONTS_FOLDER, fontName);

		char glyphPath[260];
		snprintf(glyphPath, sizeof(glyphPath), "%s%s.fnt", FONTS_FOLDER, fontName);

		auto font = std::make_unique<Font>(fontName, textureFile, texturePath, LoadGlyphs(glyphPath));
		mFonts.push_back(std::move(font));
		return mFonts.back();
	}
	catch (const std::exception& e) {
		SDL_LogError(0, "%s", e.what());
		return nullptr;
	}
}

void TextRenderer::Write(const Font& font, const char* text, Vec2 pos, const TextStyle& style, DrawOrderType drawOrder) const {
	assert(text);

	const float fontTexWidth = static_cast<float>(font.GetSurface().Width());
	const float fontTexHeight = static_cast<float>(font.GetSurface().Height());

	DrawCall drawCall;
	drawCall.program = mProgramHandle;
	drawCall.mesh = quadMesh;
	drawCall.texture = font.GetSurface().GetTextureId();
	drawCall.blending = true;

	int advance = 0;
	for (; *text; ++text) {
		const Glyph& g = font.FindGlyph(*text);

		float left = pos.x + static_cast<float>(g.xoffset + advance);
		float top = pos.y + static_cast<float>(g.yoffset);
		float right = left + static_cast<float>(g.width);
		float bottom = top + static_cast<float>(g.height);

		Rect uvs;
		uvs.left = static_cast<float>(g.x) / fontTexWidth;
		uvs.right = static_cast<float>(g.x + g.width) / fontTexWidth;
		uvs.top = static_cast<float>(g.y) / fontTexHeight;
		uvs.bottom = static_cast<float>(g.y + g.height) / fontTexHeight;

		const int   uniforms[] = { mOutlineColor, mColor, mPosRect, mUVRect, mRotation };
		const float uniformData[][4] = {
			{ style.outlineColor.r / 255.f, style.outlineColor.g / 255.f, style.outlineColor.b / 255.f, style.outlineColor.a / 255.f },
			{ style.color.r / 255.f, style.color.g / 255.f, style.color.b / 255.f, style.color.a / 255.f },
			{ left, top, right, bottom },
			{ uvs.left, uvs.top, uvs.right, uvs.bottom },
			{ 1.f, 0.f, 0.f, 0.f },
		};

		drawCall.drawOrder = drawOrder;
		drawCall.uniforms = uniforms;
		drawCall.uniformData = reinterpret_cast<const float*>(uniformData);
		drawCall.numUniforms = 5;
		mGraphics.Draw(drawCall);

		advance += g.xadvance;
	}
}

void TextRenderer::WriteAligned(const Font& font, const char* text, Vec2 pos, TextAlignment horizontalAlignment, const TextStyle& style,
                                DrawOrderType drawOrder) const {
	if (horizontalAlignment == TextAlignment::center) {
		pos.x += 0.5f * (mGraphics.GetTargetWidth() - font.CalculateStringWidth(text));
	}
	else if (horizontalAlignment == TextAlignment::right) {
		pos.x += (mGraphics.GetTargetWidth() - font.CalculateStringWidth(text));
	}
	Write(font, text, pos, style, drawOrder);
}

} // namespace Wind
