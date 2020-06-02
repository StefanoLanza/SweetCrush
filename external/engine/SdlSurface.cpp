#include "SdlSurface.h"
#include "Gl.h"
#include <SDL2/include/SDL.h>
#include <SDL2_image/include/SDL_image.h>
#include <stdexcept>
#include <string>

namespace Wind {

SdlSurface::SdlSurface(const char* fileName, const char* path)
    : mSurface(IMG_Load(path), SDL_FreeSurface)
    , mFileName(fileName) {
	if (mSurface == nullptr) {
		throw std::runtime_error(std::string("Unable to load texture ") + fileName);
	}

	GLuint textureId = 0;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	int mode;
	switch (mSurface->format->BytesPerPixel) {
	case 4:
		mode = GL_RGBA;
		break;
	case 3:
		mode = GL_RGB;
		break;
	case 1:
		mode = GL_LUMINANCE_ALPHA;
		break;
	default:
		throw std::runtime_error("Image with unknown channel profile");
	}
	glTexImage2D(GL_TEXTURE_2D, 0, mode, mSurface->w, mSurface->h, 0, mode, GL_UNSIGNED_BYTE, mSurface->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	mTextureId.reset(textureId);
}

const std::string& SdlSurface::GetFileName() const {
	return mFileName;
}

int SdlSurface::Width() const {
	return mSurface->w;
}

int SdlSurface::Height() const {
	return mSurface->h;
}

GLuint SdlSurface::GetTextureId() const {
	return mTextureId.get();
}

SdlSurface::operator SDL_Surface*() const {
	return mSurface.get();
}
} // namespace Wind
