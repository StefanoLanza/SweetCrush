#pragma once

#include <memory>
#include <string>

struct Mix_Chunk;

namespace Wind {
	class SdlSound {
	public:
		SdlSound(const char* filename, const char* path);
		const std::string& GetFileName() const;
		operator Mix_Chunk *() const;
		void Play();
		void Stop();

	private:
		std::unique_ptr<Mix_Chunk, void(*)(Mix_Chunk *)> mSound;
		std::string mFileName;
		int mChannel;
	};
}
