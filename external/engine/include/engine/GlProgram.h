#pragma once

#include "Gl.h"

#include <string>

namespace Wind {

struct GlUniform {
	char   name[32];
	GLint  location;
	GLenum type;
};

class GlProgram final {
public:
	GlProgram(const char* vertexShaderSource, const char* fragmentShaderSource, const char* defines = "");

	bool   Compile();
	GLuint GetProgramId() const;
	GLint  GetAttribLocation(const char* attrib) const;
	GLint  GetUniformLocation(const char* uniform) const;
	GLint  TryGetUniformLocation(const char* uniform) const;
	GLint  GetOrthoMatrixUniform() const;
	bool   IsEqual(const char* vertexShaderSource, const char* fragmentShaderSource, std::string_view defines) const;
	operator bool() const;
	int GetUniformCount() const;
	GlUniform QueryUniform(int index) const;

private:
	std::string      mVertexShaderSource;
	std::string      mFragmentShaderSource;
	std::string      mDefines;
	uint64_t         mHash;
	GLManagedProgram mProgram;

	// Fixed uniformLocations
	GLint mOrthoMatrixUniform;
};

} // namespace Wind
