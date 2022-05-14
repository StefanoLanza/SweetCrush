#include "SdlSound.h"
#include <SDL2/SDL_mixer.h>
#include <stdexcept>
#include <string>

namespace Wind {

SdlSound::SdlSound(const char* filename, const char* path)
    : mSound(Mix_LoadWAV(path), Mix_FreeChunk)
    , mFileName(filename)
    , mChannel(-1) {
	if (! mSound) {
		throw std::runtime_error(std::string("Unable to load sound ") + filename);
	}
}

void SdlSound::Play() {
	mChannel = Mix_PlayChannel(-1, mSound.get(), 0);
}

void SdlSound::Stop() {
	if (mChannel != -1) {
		Mix_HaltChannel(mChannel);
		mChannel = -1;
	}
}

const std::string& SdlSound::GetFileName() const {
	return mFileName;
}

SdlSound::operator Mix_Chunk*() const {
	return mSound.get();
}
} // namespace Wind
