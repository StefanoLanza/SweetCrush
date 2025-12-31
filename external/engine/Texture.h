#pragma once

#include "Gl.h"
#include <string>

namespace Wind {

enum class TextureWrapMode : GLint {
	clamp = GL_CLAMP_TO_EDGE,
	wrap = GL_REPEAT,
	mirrored = GL_MIRRORED_REPEAT,
	border = GL_CLAMP_TO_BORDER,
};

struct TextureInfo {
	TextureWrapMode wrapMode = TextureWrapMode::clamp;
	float           borderColor[4] = { 0.f, 0.f, 0.f, 0.f };
	bool            mipmaps = false;
};

class Texture final {
public:
	Texture(std::string_view filename, std::string_view path, TextureInfo info = TextureInfo {});
	const std::string& GetFileName() const;
	int                Width() const;
	int                Height() const;
	GLuint             GetTextureId() const;
	bool               HasAlpha() const;

private:
	std::string      mFileName;
	GLManagedTexture mTextureId;
	int              mWidth;
	int              mHeight;
	bool             mHasAlpha;
};

} // namespace Wind
