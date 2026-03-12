#include "GlFrameBuffer.h"
#include <SDL3/SDL.h>
#include <cassert>
#include <stdexcept>

namespace Wind {

GlFrameBuffer::GlFrameBuffer()
    : mWidth(0)
    , mHeight(0) {
}

GlFrameBuffer::GlFrameBuffer(int width, int height)
    : mWidth(width)
    , mHeight(height) {
}

GlFrameBuffer::GlFrameBuffer(int width, int height, GLuint fbo, GLuint colorTexture, GLuint depthTexture)
    : mFBO(fbo)
    , mColor(colorTexture)
    , mDepth(depthTexture)
    , mWidth(width)
    , mHeight(height) {
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
