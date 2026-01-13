#pragma once

#include "FwdDecl.h"
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

	void Blit(const GlFrameBuffer& srcFrameBuffer, BlitFilter filter) const;
	Vec2 WindowToFrameBuffer(Vec2 winCoord, const GlFrameBuffer& frameBuffer) const;

private:
	class Impl;
	std::unique_ptr<Impl> mPimpl;
};

} // namespace Wind
