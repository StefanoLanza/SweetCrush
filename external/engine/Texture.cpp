#include "Texture.h"
#include "Gl.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <cmath>
#include <stdexcept>
#include <string>

namespace Wind {

Texture::Texture(SDL_Surface* surface, std::string_view fileName, std::string_view path, TextureInfo info)
    : mFileName(fileName)
    , mHasAlpha { false } {
	GLuint textureId = 0;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);

	int  mode = GL_RGBA;
	int  internalFormat = GL_RGBA8;
	auto formatDetails = SDL_GetPixelFormatDetails(surface->format);
	switch (formatDetails->bytes_per_pixel) {
	case 4:
		mode = GL_RGBA;
		internalFormat = GL_RGBA8;
		mHasAlpha = true;
		break;
	case 3:
		mode = GL_RGB;
		internalFormat = GL_RGB8;
		break;
	case 2:
		mode = GL_RG;
		internalFormat = GL_RG8;
		break;
	case 1:
		mode = GL_LUMINANCE_ALPHA;
		internalFormat = GL_R8;
		SDL_LogError(0, "Unsupported image format ");
		break;
	default:
		SDL_LogError(0, "Image with unknown channel profile (%s)", fileName.data());
		throw std::runtime_error("Image with unknown channel profile");
	}
	int levels = 1;
	if (info.mipmaps) {
		levels = (int)std::floor(std::log2(std::max(surface->w, surface->h))) + 1;
	}
	// Tell OpenGL the row alignment matches SDL's pitch
	glTexStorage2D(GL_TEXTURE_2D, levels, internalFormat, surface->w, surface->h);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, surface->w, surface->h, mode, GL_UNSIGNED_BYTE, surface->pixels);
	if (info.mipmaps) {
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(info.filtering));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(info.filtering));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(info.wrapMode));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(info.wrapMode));
	if (info.wrapMode == TextureWrapMode::border) {
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, info.borderColor);
	}

	if (auto err = glGetError(); err != GL_NO_ERROR) {
		SDL_DestroySurface(surface);
		glDeleteTextures(1, &textureId);
		SDL_LogError(0, "GL Error. Code: %d", err);
		throw std::runtime_error("GL Error");
	}
	else {
		SDL_LogInfo(0, "Loaded image %s", path.data());
		mWidth = surface->w;
		mHeight = surface->h;
		mTextureId.reset(textureId);
	}
}

const std::string& Texture::GetFileName() const {
	return mFileName;
}

int Texture::Width() const {
	return mWidth;
}

int Texture::Height() const {
	return mHeight;
}

GLuint Texture::GetTextureId() const {
	return mTextureId.get();
}

bool Texture::HasAlpha() const {
	return mHasAlpha;
}

} // namespace Wind
