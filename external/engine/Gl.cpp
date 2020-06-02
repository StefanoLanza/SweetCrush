#include "Gl.h"

namespace Wind {

void DeleteFBO(GLuint FBO) {
	if (FBO) {
		glDeleteFramebuffers(1, &FBO);
	}
}

void DeleteTexture(GLuint texture) {
	if (texture) {
		glDeleteTextures(1, &texture);
	}
}

void DeleteProgram(GLuint program) {
	glDeleteProgram(program);
}

void DeleteShader(GLuint shader) {
	glDeleteShader(shader);
}

} // namespace Wind