#pragma once

#include "Gl.h"
#include <string>

struct SDL_Surface;

namespace Wind {

enum class TextureWrapMode {
	clamp = GL_CLAMP_TO_EDGE,
	wrap = GL_REPEAT,
	mirrored = GL_MIRRORED_REPEAT,
	border = GL_CLAMP_TO_BORDER,
};

enum class TextureFiltering {
	nearest = GL_NEAREST,
	linear = GL_LINEAR,
	nearestMipmapNearest = GL_NEAREST_MIPMAP_NEAREST,
	linearMipmapNearest = GL_LINEAR_MIPMAP_NEAREST,
	nearestMipmapLinear = GL_NEAREST_MIPMAP_LINEAR,
	linearMipmapLinear = GL_LINEAR_MIPMAP_LINEAR,
};

struct TextureInfo {
	TextureWrapMode  wrapMode = TextureWrapMode::clamp;
	TextureFiltering filtering = TextureFiltering::linear;
	float            borderColor[4] = { 0.f, 0.f, 0.f, 0.f };
	bool             mipmaps = false;
};

class Texture final {
public:
	Texture(SDL_Surface* surface, std::string_view filename, std::string_view path, TextureInfo info = TextureInfo {});
	const std::string& GetFileName() const;
	int                Width() const;
	int                Height() const;
	GLuint             GetGLId() const;
	bool               HasAlpha() const;

private:
	std::string      mFileName;
	GLManagedTexture mTextureId;
	int              mWidth;
	int              mHeight;
	bool             mHasAlpha;
};

} // namespace Wind
