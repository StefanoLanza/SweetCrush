#pragma once

#include "Gl.h"

namespace Wind {

class GlProgram {
public:
	GlProgram(const char* vertexShaderSource = nullptr, const char* fragmentShaderSource = nullptr);

	bool   Compile();
	GLuint GetProgramId() const;
	int    GetAttribLocation(const char* attrib) const;
	int    GetUniformLocation(const char* uniform) const;
	GLint  GetOrthoMatrixUniform() const;
	       operator bool() const;

private:
	const char*      mVertexShaderSource;
	const char*      mFragmentShaderSource;
	GLManagedProgram mProgram;

	// Fixed uniforms
	GLint mOrthoMatrixUniform;
};

} // namespace Wind
