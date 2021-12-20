#include "Sdl.h"
#include <SDL2/include/SDL.h>
#include <SDL_mixer/include/SDL_mixer.h>
#include <stdexcept>

namespace Wind {

Sdl::Sdl(int flags) {
	if (SDL_Init(flags) != 0) {
		const char* error = SDL_GetError();
		SDL_LogError(0, "%s", error);
		throw std::runtime_error("Failed to init SDL");
	}
	// Initialize SDL_mixer
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) == -1) {
		throw std::runtime_error("Failed to init SDL mixer");
	}
#if defined(__ANDROID__)
	/*SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);*/
#else
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#endif
}
Sdl::~Sdl() {
	SDL_Quit();
}

} // namespace Wind
