#pragma once

#include <memory>

struct _Mix_Music;
typedef struct _Mix_Music Mix_Music;

namespace Wind {

class SdlMusic {
public:
	SdlMusic(const char* filename);
	     operator Mix_Music*() const;
	void Play() const;
	void Stop() const;
	void Pause() const;
	void Resume() const;

private:
	std::unique_ptr<Mix_Music, void (*)(Mix_Music*)> mMusic;
};

} // namespace Wind
