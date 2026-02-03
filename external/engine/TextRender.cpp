#include "TextRender.h"
#include "Config.h"
#include "Font.h"
#include "Gl.h"
#include "GlProgram.h"
#include "Graphics.h"
#include "SdlWindow.h"
#include "Texture.h"
#include <SDL3/SDL.h>
#include <cassert>

namespace Wind {

TextRenderer::TextRenderer(Graphics& graphics)
    : mGraphics { graphics }
    , mProgramHandle { graphics.NewProgram(SHADERS_FOLDER "font.vs", SHADERS_FOLDER "font.fs") }
    , mValidProgram { false } {
	if (mProgramHandle != nullProgram) {
		const GlProgram& program = graphics.GetProgram(mProgramHandle);
		mPosRect = program.GetAttribLocation("posRect");
		mColor = program.GetUniformLocation("color");
		mOutlineColor = program.GetUniformLocation("outlineColor");
		mTexture = program.GetUniformLocation("inputTexture");
		mValidProgram = (mPosRect >= 0 && mColor >= 0 && mOutlineColor >= 0 && mTexture >= 0);
	}

	PipelineState pipelineState;
	pipelineState.mDepthEnabled = false;
	pipelineState.mBlending = true;
	pipelineState.mScissorTestEnabled = false;
	mPipeline = mGraphics.NewPipeline(pipelineState);
}

TextRenderer::~TextRenderer() = default;

FontPtr TextRenderer::AddFont(const char* fontName) {
	try {
		for (const FontPtr& font : mFonts) {
			if (font->GetName() == fontName) {
				return font;
			}
		}

		char texturePath[260];
		snprintf(texturePath, sizeof(texturePath), "%s%s_0.png", FONTS_FOLDER, fontName);

		char glyphPath[260];
		snprintf(glyphPath, sizeof(glyphPath), "%s%s%s.fnt", ASSETS_FOLDER, FONTS_FOLDER, fontName);

		TexturePtr texture = mGraphics.LoadTexture(texturePath);

		mFonts.emplace_back(std::make_unique<Font>(fontName, texture, LoadGlyphs(glyphPath)));
		return mFonts.back();
	}
	catch (const std::exception& e) {
		SDL_LogError(0, "%s", e.what());
		return nullptr;
	}
}

void TextRenderer::Write(const Font& font, std::string_view text, Vec2 pos, const TextStyle& style, unsigned drawOrder) const {
	if (! mValidProgram) {
		return;
	}

	struct Char {
		Rect quad;
		Rect uvs;
	};
	InstanceData instanceData = mGraphics.AllocInstances((unsigned)text.length(), sizeof(Char), mPosRect);
	if (! instanceData.data) {
		return;
	}

	const float fontTexWidth = static_cast<float>(font.GetTexture().Width());
	const float fontTexHeight = static_cast<float>(font.GetTexture().Height());
	Char*       chars = static_cast<Char*>(instanceData.data);
	for (int idx = 0, advance = 0; idx < (int)text.length(); ++idx) {
		const Glyph& g = font.FindGlyph(text[idx]);
		chars[idx].quad = {
			pos.x + static_cast<float>(g.xoffset + advance),
			pos.y + static_cast<float>(g.yoffset),
			static_cast<float>(g.width),
			static_cast<float>(g.height),
		};
		chars[idx].uvs = {
			static_cast<float>(g.x) / fontTexWidth,
			static_cast<float>(g.y) / fontTexHeight,
			static_cast<float>(g.width) / fontTexWidth,
			static_cast<float>(g.height) / fontTexHeight,
		};
		advance += g.xadvance;
	}

	const int   uniforms[] = { mColor, mOutlineColor };
	const float uniformData[][4] = {
		{ style.color.r / 255.f, style.color.g / 255.f, style.color.b / 255.f, style.color.a / 255.f },
		{ style.outlineColor.r / 255.f, style.outlineColor.g / 255.f, style.outlineColor.b / 255.f, style.outlineColor.a / 255.f },
	};
	mGraphics.SetPipeline(mPipeline);

	const unsigned textureIds[] = { font.GetTexture().GetTextureId() };

	DrawCall drawCall;
	drawCall.uniformLocations = uniforms;
	drawCall.uniforms = reinterpret_cast<const float*>(uniformData);
	drawCall.numUniforms = sizeof(uniformData) / 16;
	drawCall.textures = textureIds;
	drawCall.numTextures = 1;
	drawCall.program = mProgramHandle;
	drawCall.mesh = quadMesh;
	drawCall.drawOrder = drawOrder;
	drawCall.instances = instanceData;
	mGraphics.Draw(drawCall);
}

void TextRenderer::WriteAligned(const Font& font, std::string_view text, Vec2 pos, TextAlignment horizontalAlignment, const TextStyle& style,
                                unsigned drawOrder) const {
	if (horizontalAlignment == TextAlignment::center) {
		pos.x += 0.5f * (mGraphics.GetTargetWidth() - font.CalculateStringWidth(text));
	}
	else if (horizontalAlignment == TextAlignment::right) {
		pos.x += (mGraphics.GetTargetWidth() - font.CalculateStringWidth(text));
	}
	Write(font, text, pos, style, drawOrder);
}

} // namespace Wind
