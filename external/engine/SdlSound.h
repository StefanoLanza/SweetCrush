#pragma once

#include <memory>
#include <string>

#include <SDL3_mixer/SDL_mixer.h>

namespace Wind {

class SdlSound final {
public:
	
	SdlSound(MIX_Mixer* mixer, const char* filename, const char* path);
	const std::string& GetFileName() const;
	                   operator MIX_Audio*() const;
	void               Play();
	void               Stop();

private:
	MIX_Mixer* mMixer;
	std::unique_ptr<MIX_Audio, void (*)(MIX_Audio*)> mSound;
	std::string                                      mFileName;
};
} // namespace Wind
