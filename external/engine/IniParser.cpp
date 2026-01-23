#include "IniParser.h"

#include <SDL3/SDL.h>
#include <vector>
#include <cassert>

namespace Wind {

int ParseINIFile(const char* fileName, ini_handler handler, void* user) {
	SDL_IOStream* const f = SDL_IOFromFile(fileName, "rb");
	if (! f) {
		return -1;
	}
	int          res = -1;
	const Sint64 length = SDL_GetIOSize(f);
	if (length > 0) {
		std::vector<char> fileData(static_cast<size_t>(length) + 1);
		SDL_ReadIO(f, fileData.data(), fileData.size());
		fileData[length] = 0; // null terminate
		res = ini_parse_string(fileData.data(), handler, user);
		fileData.back() = 0; // null terminate
	}
	SDL_CloseIO(f);
	return res;
}

void INIParser::AddListener(const char* section, INIListener&& listener) {
	assert(section);
	mListeners.emplace_back(section, std::move(listener));
}

int INIParser::ParseFile(const char* fileName) const {
	SDL_IOStream* const f = SDL_IOFromFile(fileName, "rb");
	if (! f) {
		return -1;
	}
	int          res = -1;	
	const Sint64 length = SDL_GetIOSize(f);
	if (length > 0) {
		std::vector<char> fileData(static_cast<size_t>(length) + 1);
		SDL_ReadIO(f, fileData.data(), fileData.size());
		fileData[length] = 0; // null terminate
		res = ini_parse_string(fileData.data(), Handler, const_cast<INIParser*>(this));
		fileData.back() = 0; // null terminate
	}
	SDL_CloseIO(f);
	return res;
}

int INIParser::Handler(void* user, const char* section, const char* name, const char* value) {
	auto self = static_cast<INIParser*>(user);
	for (auto&& l : self->mListeners) {
		if (! strcmp(l.first, section)) {
			l.second(name, value);
			return 1; // handled
		}
	}
	return 0;
}

} // namespace Wind
