#pragma once

#include "FwdDecl.h"
#include <memory>

namespace Wind {

class Audio {
public:
	Audio();
	~Audio();

	MusicPtr LoadMusic(const char* fileName);
	SoundPtr LoadSound(const char* fileName);

private:
	class Impl;
	std::unique_ptr<Impl> mPimpl;
};

} // namespace Wind