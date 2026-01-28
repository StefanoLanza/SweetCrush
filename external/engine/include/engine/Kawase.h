#pragma once

#include "FwdDecl.h"
#include <memory>

namespace Wind {

class KawaseBlur final {
public:
	explicit KawaseBlur(Graphics& graphics);
	~KawaseBlur();

	void Run(const GlFrameBuffer& src, const GlFrameBuffer* mipChain[], int numPasses) const;

private:
	class Impl;
	std::unique_ptr<Impl> mPimpl;
};

} // namespace Wind
