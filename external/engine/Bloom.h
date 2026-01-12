#pragma once

#include "FwdDecl.h"
#include <memory>

namespace Wind {

class Bloom final {
public:
	explicit Bloom(Graphics& graphics);
	~Bloom();

private:
	class Impl;
	std::unique_ptr<Impl> mPimpl;
};

} // namespace Wind
