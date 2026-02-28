#pragma once

#include "Gl.h"

namespace Wind {

enum FBOFlags : unsigned {
	color = 0b1,
	depthStencil = 0b10,
};

class GlFrameBuffer final {
public:
	GlFrameBuffer(int width, int height);
	GlFrameBuffer(int width, int height, unsigned flags);
	GLuint GetFBO() const;
	GLuint GetColorAttachment() const;
	GLuint GetDepthAttachment() const;
	int    GetWidth() const;
	int    GetHeight() const;

	operator bool() const;

private:
	GLManagedFBO          mFBO;
	GLManagedTexture      mColor;
	GLManagedRenderBuffer mDepth;
	int                   mWidth;
	int                   mHeight;
};

} // namespace Wind
