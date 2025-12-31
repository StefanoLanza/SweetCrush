#include "Texture.h"
#include "Gl.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <cmath>
#include <stdexcept>
#include <string>

namespace Wind {

Texture::Texture(std::string_view fileName, std::string_view path, bool generateMipmaps)
    : mFileName(fileName)
    , mHasAlpha { false } {
	SDL_Surface* surface = IMG_Load(path.data());
	if (surface == nullptr) {
		SDL_LogError(0, "Unable to load image %s", fileName.data());
		throw std::runtime_error(std::string("Unable to load image ") + std::string(fileName));
	}

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
		break;
	default:
		SDL_LogError(0, "Image with unknown channel profile (%s)", fileName.data());
		throw std::runtime_error("Image with unknown channel profile");
	}
#if ! defined(__ANDROID__)
	if (glTexStorage2D)
#endif
	{
		int levels = 1;
		if (generateMipmaps) {
			levels = (int)std::floor(std::log2(std::max(surface->w, surface->h))) + 1;
		}
		glTexStorage2D(GL_TEXTURE_2D, levels, internalFormat, surface->w, surface->h);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, surface->w, surface->h, mode, GL_UNSIGNED_BYTE, surface->pixels);
#if ! defined(__ANDROID__)
	}
	else {
		glTexImage2D(GL_TEXTURE_2D, 0, mode, surface->w, surface->h, 0, mode, GL_UNSIGNED_BYTE, surface->pixels);
	}
#endif
	if (generateMipmaps) {
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	if (auto err = glGetError(); err != GL_NO_ERROR) {
		SDL_LogError(0, "GL Error. Code: %d", err);
		mWidth = 0;
		mHeight = 0;
		mHasAlpha = false;
		glDeleteTextures(1, &textureId);
	}
	else {
		SDL_LogInfo(0, "Loaded image %s", path.data());
		mWidth = surface->w;
		mHeight = surface->h;
		mTextureId.reset(textureId);
	}

	SDL_DestroySurface(surface);
	surface = nullptr;
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
