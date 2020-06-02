#pragma once

#include "Gl.h"
#include <memory>
#include <string>

struct SDL_Surface;

namespace Wind {
class SdlSurface {
public:
	SdlSurface(const char* filename, const char* path);
	const std::string& GetFileName() const;
	                   operator SDL_Surface*() const;
	int                Width() const;
	int                Height() const;
	GLuint             GetTextureId() const;

private:
	std::unique_ptr<SDL_Surface, void (*)(SDL_Surface*)> mSurface;
	std::string                                          mFileName;
	GLManagedTexture                                     mTextureId;
};
} // namespace Wind
