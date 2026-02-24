#pragma once

#include "FwdDecl.h"
#include <memory>

namespace Wind {

class HalfTone final {
public:
	explicit HalfTone(Graphics& graphics);
	~HalfTone();

	void Run(const GlFrameBuffer& src, const GlFrameBuffer& dst) const;

private:
	class Impl;
	std::unique_ptr<Impl> mPimpl;
};

} // namespace Wind
