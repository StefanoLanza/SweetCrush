#include "GlFrameBuffer.h"
#include <SDL.h>
#include <cassert>
#include <stdexcept>

namespace Wind {

GlFrameBuffer::GlFrameBuffer(int width, int height)
    : mFBO(0)
    , mTexture(0)
    , mWidth(width)
    , mHeight(height) {
	GLuint FBO = 0;
	glGenFramebuffers(1, &FBO);
	if (0 == FBO) {
		SDL_LogError(0, "Cannot create FBO");
		throw std::runtime_error("Cannot create FBO");
	}

	// Generate texture
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Bind texture to frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		throw std::runtime_error("Framebuffer is not complete");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	mFBO.reset(FBO);
	mTexture.reset(texture);
}

GLuint GlFrameBuffer::GetFBO() const {
	return mFBO.get();
}

GLuint GlFrameBuffer::GetTexture() const {
	return mTexture.get();
}

int GlFrameBuffer::GetWidth() const {
	return mWidth;
}

int GlFrameBuffer::GetHeight() const {
	return mHeight;
}

GlFrameBuffer::operator bool() const {
	GLuint FBO = mFBO.get(); // resolve ambiguity in operator !=
	return FBO != 0;
}

} // namespace Wind
