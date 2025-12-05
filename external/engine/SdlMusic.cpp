#include "SdlMusic.h"
#include <SDL3_mixer/SDL_mixer.h>
#include <stdexcept>
#include <string>

namespace Wind {

SdlMusic::SdlMusic(MIX_Mixer* mixer, const char* filename)
    : mMixer(mixer)
	, mMusic(MIX_LoadAudio(mixer, filename, false), MIX_DestroyAudio) {
	if (! mMusic) {
		throw std::runtime_error(std::string("Unable to load music ") + filename);
	}
	mTrack = MIX_CreateTrack(mixer);
	MIX_SetTrackAudio(mTrack, mMusic.get());
}

SdlMusic::~SdlMusic() {
	MIX_DestroyTrack(mTrack);
}

void SdlMusic::Play() const {
	if (!MIX_PlayTrack(mTrack, 0)) {
        SDL_LogError(0, "MIX_PlayTrack failed: %s\n", SDL_GetError());
    }
}

void SdlMusic::Stop() const {
	if (!MIX_StopTrack(mTrack, 0)) {
        SDL_LogError(0, "MIX_StopTrack failed: %s\n", SDL_GetError());
    }
}

void SdlMusic::Pause() const {
	if (!MIX_PauseTrack(mTrack)) {
        SDL_LogError(0, "MIX_PauseTrack failed: %s\n", SDL_GetError());
    }
}

void SdlMusic::Resume() const {
	if (!MIX_ResumeTrack(mTrack)) {
        SDL_LogError(0, "MIX_ResumeTrack failed: %s\n", SDL_GetError());
    }
}

SdlMusic::operator MIX_Audio *() const {
	return mMusic.get();
}

} // namespace Wind
