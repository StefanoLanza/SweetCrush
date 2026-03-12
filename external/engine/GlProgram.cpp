#include "GlProgram.h"

#include "Config.h"
#include "Hash.h"
#include <SDL3/SDL.h>
#include <cassert>
#include <malloc.h>
#include <vector>

namespace Wind {

namespace {

#define SHADERS_FOLDER ASSETS_FOLDER "shaders/"

GLuint CompileShader(const char* sources[], int numSources, GLenum type) {
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, numSources, sources, nullptr);
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

GLuint CompileShaderFromFile(const char* fileName, const char* defines, GLenum type) {
	assert(fileName);

	const char* version = "#version 310 es\n";

	char path[256];
	SDL_snprintf(path, sizeof path, "%s%s", SHADERS_FOLDER, fileName);

	GLuint              program = 0;
	SDL_IOStream* const f = SDL_IOFromFile(path, "rb");
	if (f) {
		const Sint64 length = SDL_GetIOSize(f);
		if (length > 0) {
			std::vector<char> fileData(static_cast<size_t>(length) + 1);
			SDL_ReadIO(f, fileData.data(), fileData.size());
			fileData.back() = 0; // null terminate
			const char* sources[] = {
				version,
				defines,
				fileData.data(),
			};
			program = CompileShader(sources, 3, type);
		}
		else {
			SDL_LogError(0, "Zero length file %s", path);
		}
		SDL_CloseIO(f);
	}
	else {
		SDL_LogError(0, "Cannot open file %s", path);
	}
	return program;
}

bool Validate(GLuint program) {
	GLint logLength;
	glValidateProgram(program);
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 0) {
		GLchar* log = (GLchar*)malloc(logLength);
		glGetProgramInfoLog(program, logLength, &logLength, log);
		SDL_LogError(0, "%s", log);
		free(log);
	}
	return status == GL_TRUE;
}

} // namespace

GlProgram::GlProgram(const char* vertexShaderSource, const char* fragmentShaderSource, const char* defines)
    : mVertexShaderSource { vertexShaderSource }
    , mFragmentShaderSource { fragmentShaderSource }
    , mDefines { defines }
    , mHash { 0 }
    , mOrthoMatrixUniform { -1 } {
	mHash = Hash(vertexShaderSource) | Hash(fragmentShaderSource);
	if (defines) {
		mHash |= Hash(defines);
	}
}

bool GlProgram::Compile() {
	bool             valid = false;
	const GLuint     vertexShader = CompileShaderFromFile(mVertexShaderSource.c_str(), mDefines.c_str(), GL_VERTEX_SHADER);
	const GLuint     fragmentShader = CompileShaderFromFile(mFragmentShaderSource.c_str(), mDefines.c_str(), GL_FRAGMENT_SHADER);
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
			valid = Validate(program.get());
		}
		else {
			SDL_LogError(0, "Error linking program (vs: %s fs: %s)", mVertexShaderSource.c_str(), mFragmentShaderSource.c_str());
		}
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	if (valid) {
		mProgram = std::move(program);
		// Get default uniformLocations
		mOrthoMatrixUniform = TryGetUniformLocation("orthoMatrix");
	}

	if (auto err = glGetError(); err != GL_NO_ERROR) {
		SDL_LogError(0, "GL Error. Code: %d", err);
		valid = false;
	}

	if (valid) {
		SDL_LogInfo(0, "Compiled GL Program (vs: %s fs: %s)", mVertexShaderSource.c_str(), mFragmentShaderSource.c_str());
	}

	// else keep current program
	return valid;
}

GLuint GlProgram::GetProgramId() const {
	return mProgram.get();
}

GLint GlProgram::GetAttribLocation(const char* attrib) const {
	assert(attrib);
	if (mProgram == 0) {
		SDL_LogError(0, "%s is not a valid glsl program attribute", attrib);
		return -1;
	}
	int location = glGetAttribLocation(mProgram.get(), attrib);
	if (location == -1) {
		SDL_LogError(0, "%s is not a valid glsl program attribute", attrib);
	}
	return location;
}

GLint GlProgram::GetUniformLocation(const char* uniform) const {
	assert(uniform);
	if (mProgram == 0) {
		SDL_LogError(0, "%s is not a valid glsl program uniform", uniform);
		return -1;
	}
	int location = glGetUniformLocation(mProgram.get(), uniform);
	if (location == -1) {
		SDL_LogError(0, "%s is not a valid glsl program uniform", uniform);
	}
	return location;
}

GLint GlProgram::TryGetUniformLocation(const char* uniform) const {
	assert(uniform);
	if (mProgram == 0) {
		return -1;
	}
	int location = glGetUniformLocation(mProgram.get(), uniform);
	if (location == -1) {
	}
	return location;
}

GLint GlProgram::GetOrthoMatrixUniform() const {
	return mOrthoMatrixUniform;
}

bool GlProgram::IsEqual(const char* vertexShaderSource, const char* fragmentShaderSource, std::string_view defines) const {
	uint64_t hash = Hash(vertexShaderSource) | Hash(fragmentShaderSource);
	if (defines.empty()) {
		hash |= Hash(defines.data());
	}
	if (hash != mHash) {
		return false;
	}
	return mVertexShaderSource == vertexShaderSource && mFragmentShaderSource == fragmentShaderSource && mDefines == defines;
}

GlProgram::operator bool() const {
	GLuint program = mProgram.get();
	return program != 0;
}

int GlProgram::GetUniformCount() const {
	GLint count = 0;
	glGetProgramiv(mProgram.get(), GL_ACTIVE_UNIFORMS, &count);
	GLint maxLength = 0;
	glGetProgramiv(mProgram.get(), GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength);
	if (maxLength > (int)sizeof(GlUniform::name)) {
		SDL_LogError(0, "size of GlUniform.name is too small. Increase it to at least %d", maxLength);
		return 0;
	}
	return count;
}

GlUniform GlProgram::QueryUniform(int index) const {
	GLint     size;
	GLsizei   length;
	GlUniform u;
	glGetActiveUniform(mProgram.get(), index, sizeof(GlUniform::name), &length, &size, &u.type, u.name);
	u.location = glGetUniformLocation(mProgram.get(), u.name);
	return u;
}

} // namespace Wind
