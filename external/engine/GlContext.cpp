#include "GlContext.h"
#include "Gl.h"
#include "SdlWindow.h"
#include <glew/glew.h>
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
#if _WIN32
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK) {
		throw std::runtime_error("Error initialising glew");
	}
#endif
}

GlContext::operator SDL_GLContext() const {
	return mContext.get();
}
} // namespace Wind