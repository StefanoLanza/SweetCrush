#pragma once

#include "Gl.h"
#include <string>

namespace Wind {

class Texture final {
public:
	Texture(std::string_view filename, std::string_view path, bool generateMipmaps = true);
	const std::string& GetFileName() const;
	int                Width() const;
	int                Height() const;
	GLuint             GetTextureId() const;
	bool               HasAlpha() const;

private:
	std::string      mFileName;
	GLManagedTexture mTextureId;
	int              mWidth;
	int              mHeight;
	bool             mHasAlpha;
};

} // namespace Wind
