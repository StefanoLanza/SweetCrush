#include "GlFrameBuffer.h"
#include <SDL3/SDL.h>
#include <cassert>
#include <stdexcept>

namespace Wind {

GlFrameBuffer::GlFrameBuffer(int width, int height)
    : mFBO(0)
    , mWidth(width)
    , mHeight(height) {
}

GlFrameBuffer::GlFrameBuffer(int width, int height, unsigned flags)
    : mFBO(0)
    , mColor(0)
    , mWidth(width)
    , mHeight(height) {
	GLuint FBO = 0;
	glGenFramebuffers(1, &FBO);
	if (0 == FBO) {
		SDL_LogError(0, "Cannot create FBO");
		throw std::runtime_error("Cannot create FBO");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	GLuint colorTexture = 0;
	// Generate color texture attachment
	if (flags & (unsigned)FBOFlags::color) {
		glGenTextures(1, &colorTexture);
		glBindTexture(GL_TEXTURE_2D, colorTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
		// Bind texture to frame buffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
	}

	GLuint depthRenderBuffer = 0;
	if (flags & (unsigned)FBOFlags::depthStencil) {
		glGenRenderbuffers(1, &depthRenderBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		throw std::runtime_error("Framebuffer is not complete");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0); // unbind

	if (auto err = glGetError(); err != GL_NO_ERROR) {
		SDL_LogError(0, "GL Error. Code: %d", err);
	}

	mFBO.reset(FBO);
	mColor.reset(colorTexture);
	mDepth.reset(depthRenderBuffer);
}

GLuint GlFrameBuffer::GetFBO() const {
	return mFBO.get();
}

GLuint GlFrameBuffer::GetColorAttachment() const {
	return mColor.get();
}

GLuint GlFrameBuffer::GetDepthAttachment() const {
	return mDepth.get();
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
