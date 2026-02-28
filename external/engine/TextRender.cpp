#include "TextRender.h"
#include "Font.h"
#include "Gl.h"
#include "GlProgram.h"
#include "Graphics.h"
#include "Texture.h"
#include "UI.h"

namespace Wind {

UITextRenderer::UITextRenderer(Graphics& graphics)
    : mGraphics { graphics }
    , mProgramHandle { graphics.NewProgram("ui/font.vs", "ui/font.fs") }
    , mValidProgram { false } {
	if (mProgramHandle != nullProgram) {
		const GlProgram& program = graphics.GetProgram(mProgramHandle);
		mCoords = program.GetAttribLocation("coords");
		mPosOffset = program.GetUniformLocation("posOffset");
		mColor = program.GetUniformLocation("color");
		mOutlineColor = program.GetUniformLocation("outlineColor");
		mTexture = program.GetUniformLocation("inputTexture");
		mValidProgram = (mCoords >= 0 && mPosOffset >= 0 && mColor >= 0 && mOutlineColor >= 0 && mTexture >= 0);
	}

	const PipelineState pipelineState {
		.depthEnabled = false,
		.scissorTestEnabled = false,
		.blending = true,
	};
	mPipeline = mGraphics.NewPipeline(pipelineState);

	glGenSamplers(1, &mSampler);
	glSamplerParameteri(mSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameteri(mSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(mSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(mSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

UITextRenderer::~UITextRenderer() = default;

void UITextRenderer::Write(const Font& font, std::string_view text, const UIRect& rect, const UITextStyle& style, unsigned drawOrder) const {
	if (! mValidProgram) {
		return;
	}

	float textWidth = static_cast<float>(font.CalculateStringWidth(text));
	float textHeight = static_cast<float>(font.GetHeight());
	Vec2  scale { rect.size.x / textWidth, rect.size.y / textHeight };
	WriteImpl(font, text, rect.pos, scale, style, drawOrder);
}

void UITextRenderer::Write(const Font& font, std::string_view text, Vec2 pos, const UITextStyle& style, unsigned drawOrder) const {
	if (! mValidProgram) {
		return;
	}
	WriteImpl(font, text, pos, { 1.f, 1.f }, style, drawOrder);
}

void UITextRenderer::WriteImpl(const Font& font, std::string_view text, Vec2 pos, Vec2 scale, const UITextStyle& style, unsigned drawOrder) const {
	if (! mValidProgram) {
		return;
	}
	struct Char {
		Rect quad;
		Rect uvs;
	};
	InstanceData instanceData = mGraphics.AllocInstances((unsigned)text.length(), sizeof(Char), mCoords);
	if (! instanceData.data) {
		return;
	}

	const float margin = 0.5f;
	const float fontTexWidth = static_cast<float>(font.GetTexture().Width());
	const float fontTexHeight = static_cast<float>(font.GetTexture().Height());
	Char*       chars = static_cast<Char*>(instanceData.data);
	float       advance = 0.f;
	for (int idx = 0; idx < (int)text.length(); ++idx) {
		int          ridx = idx;
		const Glyph& g = font.FindGlyph(text[ridx]);
		chars[idx].quad = {
			pos.x + static_cast<float>(g.xoffset - margin) * scale.x + advance,
			pos.y + static_cast<float>(g.yoffset - margin) * scale.y,
			static_cast<float>(g.width + 2.0 * margin) * scale.x,
			static_cast<float>(g.height + 2.0 * margin) * scale.y,
		};
		chars[idx].uvs = {
			static_cast<float>(g.x - margin) / fontTexWidth,
			static_cast<float>(g.y - margin) / fontTexHeight,
			static_cast<float>(g.width + 2.f * margin) / fontTexWidth,
			static_cast<float>(g.height + 2.f * margin) / fontTexHeight,
		};
		advance += g.xadvance * scale.x;
	}

	const int   uniforms[] = { mPosOffset, mColor, mOutlineColor };
	const float uniformData[][4] = {
		{ 0.f, 0.f, 0.f, 0.f },
		{ style.color.r / 255.f, style.color.g / 255.f, style.color.b / 255.f, style.color.a / 255.f },
		{ style.outlineColor.r / 255.f, style.outlineColor.g / 255.f, style.outlineColor.b / 255.f, style.outlineColor.a / 255.f },
	};
	const float shadowUniformData[][4] = {
		{ style.shadowOffset.x, style.shadowOffset.y, 0.f, 0.f },
		{ style.shadowColor.r / 255.f, style.shadowColor.g / 255.f, style.shadowColor.b / 255.f, style.shadowColor.a / 255.f },
		{ style.shadowColor.r / 255.f, style.shadowColor.g / 255.f, style.shadowColor.b / 255.f, 1.f },
	};
	mGraphics.SetPipeline(mPipeline);

	const unsigned textureIds[] = { font.GetTexture().GetGLId() };
	const unsigned samplers[] = { mSampler };

	DrawCall drawCall;
	drawCall.uniformLocations = uniforms;
	drawCall.numUniforms = std::size(uniforms);
	drawCall.textures = textureIds;
	drawCall.numTextures = 1;
	drawCall.samplers = samplers;
	drawCall.program = mProgramHandle;
	drawCall.mesh = quadMesh;
	drawCall.drawOrder = drawOrder;
	drawCall.instances = instanceData;

	if (style.shadow) {
		drawCall.uniforms = shadowUniformData;
		mGraphics.Draw(drawCall);
	}

	drawCall.uniforms = uniformData;
	mGraphics.Draw(drawCall);
}

} // namespace Wind
