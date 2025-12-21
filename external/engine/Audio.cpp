#include "Audio.h"
#include "Config.h"
#include "SdlMusic.h"
#include "SdlSound.h"
#include <SDL3/SDL.h>
#include <stdexcept>
#include <cassert>
#include <vector>

namespace Wind {

class Audio::Impl {
public:

	Impl();

	MusicPtr LoadMusic(const char* fileName);
	SoundPtr LoadSound(const char* fileName);

private:
	MIX_Mixer* mMixer;
	std::vector<MusicPtr> mMusics;
	std::vector<SoundPtr> mSounds;
};

Audio::Impl::Impl() {
	// Initialize SDL_mixer
	SDL_LogInfo(0, "Initializing SDL mixer");
	if (!MIX_Init()) { // initialize SDL3_mixer
		SDL_LogError(0, "%s", SDL_GetError());
	}
	// Create a mixer that outputs to the default playback device.
    MIX_Mixer *mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
    if (!mixer) {
        SDL_LogError(0, "MIX_CreateMixerDevice failed: %s\n", SDL_GetError());
        MIX_Quit();
    }

	mMixer = mixer;
}

MusicPtr Audio::Impl::LoadMusic(const char* fileName) {
	assert(fileName);
	if (! mMixer) {
		return nullptr;
	}
	try {
		char path[260];
		snprintf(path, sizeof(path), "%s%s", ASSETS_FOLDER, fileName);
		mMusics.emplace_back(std::make_shared<SdlMusic>(mMixer, path));
		return mMusics.back();
	}
	catch (const std::exception& e) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", e.what());
		return nullptr;
	}
}

SoundPtr Audio::Impl::LoadSound(const char* fileName) {
	assert(fileName);
	if (! mMixer) {
		return nullptr;
	}
	try {
		size_t idx = 0;
		for (const SoundPtr& s : mSounds) {
			if (s->GetFileName() == fileName) {
				return s;
			}
			++idx;
		}
		char path[260];
		snprintf(path, sizeof(path), "%s%s", ASSETS_FOLDER, fileName);
		mSounds.emplace_back(std::make_unique<SdlSound>(mMixer, fileName, path));
	}
	catch (...) {
		return nullptr;
	}
	return mSounds.back();
}
Audio::Audio()
    : mPimpl(std::make_unique<Impl>()) {
}

Audio::~Audio() = default;

MusicPtr Audio::LoadMusic(const char* fileName) {
	return mPimpl->LoadMusic(fileName);
}

SoundPtr Audio::LoadSound(const char* fileName) {
	return mPimpl->LoadSound(fileName);
}

} // namespace Wind
