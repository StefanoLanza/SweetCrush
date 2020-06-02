#pragma once

#include <memory>
#include "FwdDecl.h"

namespace Wind {

class Audio {
public:
	Audio();
	~Audio();

	MusicPtr  LoadMusic(const char* fileName);
	SoundPtr  LoadSound(const char* fileName);

private:
	class Impl;
	std::unique_ptr<Impl> mPimpl;
};

} // namespace Wind