#include "TextRender.h"
#include "Config.h"
#include "Font.h"
#include "Gl.h"
#include "GlProgram.h"
#include "Graphics.h"
#include "Texture.h"

namespace Wind {

UITextRenderer::UITextRenderer(Graphics& graphics)
    : mGraphics { graphics }
    , mProgramHandle { graphics.NewProgram(SHADERS_FOLDER "ui/font.vs", SHADERS_FOLDER "ui/font.fs") }
    , mValidProgram { false } {
	if (mProgramHandle != nullProgram) {
		const GlProgram& program = graphics.GetProgram(mProgramHandle);
		mPosRect = program.GetAttribLocation("coords");
		mColor = program.GetUniformLocation("color");
		mOutlineColor = program.GetUniformLocation("outlineColor");
		mTexture = program.GetUniformLocation("inputTexture");
		mValidProgram = (mPosRect >= 0 && mColor >= 0 && mOutlineColor >= 0 && mTexture >= 0);
	}

	const PipelineState pipelineState {
		.mDepthEnabled = false,
		.mScissorTestEnabled = false,
		.mBlending = true,
	};
	mPipeline = mGraphics.NewPipeline(pipelineState);

	glGenSamplers(1, &mSampler);
	glSamplerParameteri(mSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameteri(mSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(mSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(mSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

UITextRenderer::~UITextRenderer() = default;

void UITextRenderer::Write(const Font& font, std::string_view text, Vec2 pos, const TextStyle& style, TextDirection direction,
                           unsigned drawOrder) const {
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
	float       advance = 0.f;
	for (int idx = 0; idx < (int)text.length(); ++idx) {
		int ridx = idx;
		if (direction == TextDirection::rightToLeft) {
			ridx = (int)text.length() - 1 - idx;
		}
		const Glyph& g = font.FindGlyph(text[ridx]);
		chars[idx].quad = {
			pos.x + static_cast<float>(g.xoffset) + advance,
			pos.y + static_cast<float>(g.yoffset),
			static_cast<float>(g.width * style.scale),
			static_cast<float>(g.height * style.scale),
		};
		chars[idx].uvs = {
			static_cast<float>(g.x) / fontTexWidth,
			static_cast<float>(g.y) / fontTexHeight,
			static_cast<float>(g.width) / fontTexWidth,
			static_cast<float>(g.height) / fontTexHeight,
		};
		advance += g.xadvance * style.scale;
	}

	const int   uniforms[] = { mColor, mOutlineColor };
	const float uniformData[][4] = {
		{ style.color.r / 255.f, style.color.g / 255.f, style.color.b / 255.f, style.color.a / 255.f },
		{ style.outlineColor.r / 255.f, style.outlineColor.g / 255.f, style.outlineColor.b / 255.f, style.outlineColor.a / 255.f },
	};
	mGraphics.SetPipeline(mPipeline);

	const unsigned textureIds[] = { font.GetTexture().GetGLId() };
	const unsigned samplers[] = { mSampler };

	DrawCall drawCall;
	drawCall.uniformLocations = uniforms;
	drawCall.uniforms = reinterpret_cast<const float*>(uniformData);
	drawCall.numUniforms = std::size(uniforms);
	drawCall.textures = textureIds;
	drawCall.numTextures = 1;
	drawCall.samplers = samplers;
	drawCall.program = mProgramHandle;
	drawCall.mesh = quadMesh;
	drawCall.drawOrder = drawOrder;
	drawCall.instances = instanceData;
	mGraphics.Draw(drawCall);
}

void UITextRenderer::WriteAligned(const Font& font, std::string_view text, Vec2 pos, TextAlignment horizontalAlignment, TextDirection direction,
                                  const TextStyle& style, unsigned drawOrder) const {
	if (horizontalAlignment == TextAlignment::center) {
		pos.x += 0.5f * (mGraphics.GetTargetWidth() - font.CalculateStringWidth(text));
	}
	else if (horizontalAlignment == TextAlignment::right) {
		pos.x += (mGraphics.GetTargetWidth() - font.CalculateStringWidth(text));
	}
	Write(font, text, pos, style, direction, drawOrder);
}

} // namespace Wind
