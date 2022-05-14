#include "GlProgram.h"
#include <SDL2/SDL.h>
#include <cassert>
#include <malloc.h>
#include <vector>

namespace Wind {

namespace {

GLuint CompileShader(const char* source, GLenum type) {
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, nullptr);
	glCompileShader(shader);
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		GLint logLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
		if (logLength > 0) {
			GLchar* log = (GLchar*)malloc(logLength);
			glGetShaderInfoLog(shader, logLength, &logLength, log);
			SDL_LogError(0, "%s", log);
			free(log);
		}
		glDeleteShader(shader);
		shader = 0;
	}
	return shader;
}

GLuint CompileShaderFromFile(const char* source, GLenum type) {
	assert(source);

	GLuint           program = 0;
	SDL_RWops* const f = SDL_RWFromFile(source, "rb");
	if (f) {
		const Sint64 length = SDL_RWseek(f, 0, RW_SEEK_END);
		if (length > 0) {
			SDL_RWseek(f, 0, RW_SEEK_SET);
			std::vector<char> data(static_cast<size_t>(length) + 1);
			SDL_RWread(f, data.data(), data.size(), 1);
			data.back() = 0; // null terminate
			program = CompileShader(data.data(), type);
		}
		else {
			SDL_LogError(0, "Zero length file %s", source);
		}
		SDL_RWclose(f);
	}
	else {
		SDL_LogError(0, "Cannot open file %s", source);
	}
	return program;
}

void Validate(GLuint program) {
	GLint logLength;
	glValidateProgram(program);
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 0) {
		GLchar* log = (GLchar*)malloc(logLength);
		glGetProgramInfoLog(program, logLength, &logLength, log);
		SDL_LogError(0, "%s", log);
		free(log);
	}
}
} // namespace

GlProgram::GlProgram(const char* vertexShaderSource, const char* fragmentShaderSource)
    : mVertexShaderSource { vertexShaderSource }
    , mFragmentShaderSource { fragmentShaderSource } {
}

bool GlProgram::Compile() {
	bool             valid = false;
	const GLuint     vertexShader = CompileShaderFromFile(mVertexShaderSource, GL_VERTEX_SHADER);
	const GLuint     fragmentShader = CompileShaderFromFile(mFragmentShaderSource, GL_FRAGMENT_SHADER);
	GLManagedProgram program;
	if (vertexShader && fragmentShader) {
		program.reset(glCreateProgram());
		glAttachShader(program.get(), vertexShader);
		glAttachShader(program.get(), fragmentShader);
		// Link
		glLinkProgram(program.get());
		GLint status;
		glGetProgramiv(program.get(), GL_LINK_STATUS, &status);
		if (status == GL_TRUE) {
			valid = true;
		}
		else {
			SDL_LogError(0, "Error linking program");
		}
		Validate(program.get());
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	if (valid) {
		mProgram = std::move(program);
		mOrthoMatrixUniform = GetUniformLocation("orthoMatrix");
	}
	// else keep current program
	return valid;
}

GLuint GlProgram::GetProgramId() const {
	return mProgram.get();
}

int GlProgram::GetAttribLocation(const char* attrib) const {
	assert(attrib);
	if (mProgram == 0) {
		SDL_LogError(0, "%s is not a valid glsl program attribute", attrib);
		return -1;
	}
	return glGetAttribLocation(mProgram.get(), attrib);
}

int GlProgram::GetUniformLocation(const char* uniform) const {
	assert(uniform);
	if (mProgram == 0) {
		SDL_LogError(0, "%s is not a valid glsl program uniform variable", uniform);
		return -1;
	}
	return glGetUniformLocation(mProgram.get(), uniform);
}

GLuint GlProgram::GetOrthoMatrixUniform() const {
	return mOrthoMatrixUniform;
}

GlProgram::operator bool() const {
	GLuint program = mProgram.get();
	return program != 0;
}

} // namespace Wind
