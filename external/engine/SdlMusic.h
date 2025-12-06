#pragma once

#include <memory>

#include <SDL3_mixer/SDL_mixer.h>

namespace Wind {

class SdlMusic final {
public:
	explicit SdlMusic(MIX_Mixer* mixer, const char* filename);
	~SdlMusic();

	operator MIX_Audio*() const;
	void Play() const;
	void Stop() const;
	void Pause() const;
	void Resume() const;

private:
	MIX_Track *mTrack;
	std::unique_ptr<MIX_Audio, void (*)(MIX_Audio*)> mMusic;
};

} // namespace Wind
