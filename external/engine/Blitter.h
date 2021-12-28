#pragma once

#include "FwdDecl.h"
#include <memory>

namespace Wind {


struct Vec2;

enum class Effect { bloom };

class Blitter {
public:
	Blitter(Graphics& graphics);
	~Blitter();

	void Blit(const GlFrameBuffer& frameBuffer) const;
	Vec2 WindowToFrameBuffer(Vec2 winCoord, const GlFrameBuffer& frameBuffer) const;

private:
	class Impl;
	std::unique_ptr<Impl> mPimpl;
};

} // namespace Wind
