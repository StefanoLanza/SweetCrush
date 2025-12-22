#pragma once

#include "Gl.h"
#include <memory>
#include <string>

struct SDL_Surface;

namespace Wind {

class SdlSurface final {
public:
	SdlSurface(std::string_view filename, std::string_view path);
	const std::string& GetFileName() const;
	int                Width() const;
	int                Height() const;
	GLuint             GetTextureId() const;
	bool               HasAlpha() const;
	operator SDL_Surface*() const;

private:
	std::unique_ptr<SDL_Surface, void (*)(SDL_Surface*)> mSurface;
	std::string                                          mFileName;
	GLManagedTexture                                     mTextureId;
	bool                                                 mHasAlpha;
};

} // namespace Wind
