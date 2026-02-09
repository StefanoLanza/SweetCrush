#pragma once

#include "FwdDecl.h"
#include "Gl.h"

#include <memory>

namespace Wind {

struct Vec2;

enum class BlitFilter {
	point,
	linear,
};

class Blitter final {
public:
	explicit Blitter(Graphics& graphics);
	~Blitter();

	void Blit(GLuint srcTexture, int srcWidth, int srcHeight, BlitFilter filter) const;
	Vec2 WindowToFrameBuffer(Vec2 winCoord, int srcWidth, int srcHeight) const;

private:
	class Impl;
	std::unique_ptr<Impl> mPimpl;
};

} // namespace Wind
