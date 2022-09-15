#pragma once

#if defined(_WIN32)
#include <glew/glew.h>
#elif defined(__ANDROID__) || defined(ANDROID)
#include <SDL2/SDL_opengles2.h>
#elif defined(__linux__) 
/* Ensure we are using opengl's core profile only */
#define GL3_PROTOTYPES 1
#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL_opengl.h>
//#include <GL/gl3.h>
#else
#  error platform not supported.
#endif

#include <memory>

namespace Wind {

template <void (*func)(GLuint)>
struct gl_object_deleter {
	struct pointer {
		GLuint x;
		pointer(std::nullptr_t = nullptr)
		    : x(0) {
		}
		pointer(GLuint x)
		    : x(x) {
		}
		operator GLuint() const {
			return x;
		}
		friend bool operator==(pointer x, pointer y) {
			return x.x == y.x;
		}
		friend bool operator!=(pointer x, pointer y) {
			return x.x != y.x;
		}
	};
	void operator()(GLuint p) const {
		func(p);
	}
};

void DeleteFBO(GLuint FBO);
void DeleteTexture(GLuint texture);
void DeleteProgram(GLuint program);
void DeleteShader(GLuint shader);

// RAII wrappers
using GLManagedFBO = std::unique_ptr<GLuint, gl_object_deleter<DeleteFBO>>;
using GLManagedProgram = std::unique_ptr<GLuint, gl_object_deleter<DeleteProgram>>;
using GLManagedTexture = std::unique_ptr<GLuint, gl_object_deleter<DeleteTexture>>;
using GLManagedShader = std::unique_ptr<GLuint, gl_object_deleter<DeleteShader>>;

} // namespace Wind
