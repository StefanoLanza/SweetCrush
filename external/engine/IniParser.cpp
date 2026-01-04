#include "IniParser.h"

#include <SDL3/SDL.h>
#include <vector>

namespace Wind {

int ParseINIFile(const char* fileName, ini_handler handler, void* user) {
	SDL_IOStream* const f = SDL_IOFromFile(fileName, "rb");
	if (! f) {
		return -1;
	}
	int          res = -1;
	const Sint64 length = SDL_SeekIO(f, 0, SDL_IO_SEEK_END);
	if (length > 0) {
		SDL_SeekIO(f, 0, SDL_IO_SEEK_SET);
		std::vector<char> fileData(static_cast<size_t>(length) + 1);
		SDL_ReadIO(f, fileData.data(), fileData.size());
		fileData[length] = 0; // null terminate
		res = ini_parse_string(fileData.data(), handler, user);
		fileData.back() = 0; // null terminate
	}
	SDL_CloseIO(f);
	return res;
}

} // namespace Wind
