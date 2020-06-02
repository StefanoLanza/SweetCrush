#pragma once

#include "Gl.h"

namespace Wind {

class GlFrameBuffer {
public:
	GlFrameBuffer(int width, int height);
	GLuint GetFBO() const;
	GLuint GetTexture() const;
	int    GetWidth() const;
	int    GetHeight() const;

	operator bool() const;

private:
	GLManagedFBO     mFBO;
	GLManagedTexture mTexture;
	int              mWidth;
	int              mHeight;
};

} // namespace Wind
