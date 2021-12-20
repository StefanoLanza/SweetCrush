#include "SdlMusic.h"
#include <SDL_mixer/include/SDL_mixer.h>
#include <stdexcept>
#include <string>

namespace Wind {

SdlMusic::SdlMusic(const char* filename)
    : mMusic(Mix_LoadMUS(filename), Mix_FreeMusic) {
	if (! mMusic) {
		throw std::runtime_error(std::string("Unable to load music ") + filename);
	}
}

void SdlMusic::Play() const {
	Mix_PlayMusic(mMusic.get(), -1);
}

void SdlMusic::Stop() const {
	Mix_HaltMusic();
}

void SdlMusic::Pause() const {
	Mix_PauseMusic();
}

void SdlMusic::Resume() const {
	Mix_ResumeMusic();
}

SdlMusic::operator Mix_Music*() const {
	return mMusic.get();
}

} // namespace Wind
