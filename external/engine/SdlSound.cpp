#include "SdlSound.h"
#include <SDL3_mixer/SDL_mixer.h>
#include <stdexcept>
#include <string>

namespace Wind {

SdlSound::SdlSound(MIX_Mixer* mixer, const char* filename, const char* path)
    : mMixer(mixer)
	, mSound(MIX_LoadAudio(mixer, path, true), MIX_DestroyAudio)
    , mFileName(filename) {
	if (! mSound) {
		throw std::runtime_error(std::string("Unable to load sound ") + filename);
	}
}

void SdlSound::Play() {
	if (! MIX_PlayAudio(mMixer, mSound.get()))
	{
		SDL_LogError(0, "MIX_PlayAudio failed: %s\n", SDL_GetError());	
	}
}

void SdlSound::Stop() {
	//if (mChannel != -1) {
		//MIX_Sto
		//Mix_HaltChannel(mChannel);
		//mChannel = -1;
	//}
}

const std::string& SdlSound::GetFileName() const {
	return mFileName;
}

SdlSound::operator MIX_Audio *() const {
	return mSound.get();
}

} // namespace Wind
