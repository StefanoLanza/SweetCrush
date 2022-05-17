#include "Audio.h"
#include "Config.h"
#include "SdlMusic.h"
#include "SdlSound.h"
#include <SDL.h>
#include <cassert>
#include <vector>

namespace Wind {

class Audio::Impl {
public:
	MusicPtr LoadMusic(const char* fileName);
	SoundPtr LoadSound(const char* fileName);

private:
	std::vector<MusicPtr> mMusics;
	std::vector<SoundPtr> mSounds;
};

MusicPtr Audio::Impl::LoadMusic(const char* fileName) {
	assert(fileName);
	try {
		char path[260];
		snprintf(path, sizeof(path), "%s%s", ASSETS_FOLDER, fileName);
		SdlMusic music { path };
		mMusics.push_back(std::make_shared<SdlMusic>(std::move(music)));
		return mMusics.back();
	}
	catch (const std::exception& e) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", e.what());
		return nullptr;
	}
}

SoundPtr Audio::Impl::LoadSound(const char* fileName) {
	assert(fileName);
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
		mSounds.emplace_back(std::make_unique<SdlSound>(fileName, path));
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
