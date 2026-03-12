#pragma once

#include "FwdDecl.h"
#include <memory>

namespace Wind {

class Blur final {
public:
	explicit Blur(Graphics& graphics);
	~Blur();

	void Run(const GlFrameBuffer& src, const GlFrameBuffer& dst, const GlFrameBuffer& temp) const;

private:
	class Impl;
	std::unique_ptr<Impl> mPimpl;
};

} // namespace Wind
