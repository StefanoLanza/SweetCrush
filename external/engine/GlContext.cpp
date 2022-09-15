#include "GlContext.h"
#ifdef _WIN32
#include <glew/glew.h>
#endif
#include "Gl.h"
#include "SdlWindow.h"
#include <stdexcept>
#include <string>

namespace Wind {

GlContext::GlContext(SdlWindow& sdlWindow)
    : mContext(SDL_GL_CreateContext(sdlWindow), SDL_GL_DeleteContext) {
	if (! mContext) {
		const char* error = SDL_GetError();
		SDL_LogError(0, "%s", error);
		throw std::runtime_error(std::string("Error initialising OpenGL context: ") + error);
	}
	SDL_LogInfo(0, "GL context created");
#ifdef _WIN32
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK) {
		throw std::runtime_error("Error initialising glew");
	}
	SDL_LogInfo(0, "GLEW initialized");
#endif
}

GlContext::operator SDL_GLContext() const {
	return mContext.get();
}

} // namespace Wind
