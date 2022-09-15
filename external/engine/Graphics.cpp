#include "Graphics.h"
#include "Config.h"
#include "Gl.h"
#include "GlFrameBuffer.h"
#include "GlProgram.h"
#include "SdlSurface.h"
#include "SdlWindow.h"
#include <SDL.h>
#include <algorithm>
#include <cassert>
#include <vector>

namespace Wind {

namespace {

struct Batch {
	RectI    scissorRect;
	unsigned programIdx;
	unsigned firstUniform;
	unsigned numUniforms;
	uint32_t sortKey;
	bool     blending;
};

struct Target {
	GLuint fbo;
	int    width;
	int    height;
};

struct Mesh {
	GLuint  VAO;
	GLuint  VBO;
	GLuint  IBO;
	GLuint  numVertices;
	GLsizei numIndices;
};

enum class ShaderUniformType {
	float4,
	int4,
	texture,
};

struct ShaderUniform {
	ShaderUniformType type;
	GLint             uniform;
	union {
		float    fvalue[4];
		int      ivalue[4];
		unsigned texture;
	};
};

Mesh BuildMesh(GLuint numVertices, GLsizei numIndices, const GLfloat vertexData[], const GLuint  indexData[]) {
	constexpr GLuint numComponents = 2; // x,y for position
	Mesh mesh {};
	mesh.numIndices = numIndices;
	mesh.numVertices = numVertices;

 	glGenVertexArrays(1, &mesh.VAO);
	glBindVertexArray(mesh.VAO);

	// Generate VBO and store it in the VAO
	glGenBuffers(1, &mesh.VBO);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, numComponents, GL_FLOAT, GL_FALSE, numComponents * sizeof(GLfloat), NULL);
	glBufferData(GL_ARRAY_BUFFER, numVertices * numComponents * sizeof(GLfloat), vertexData, GL_STATIC_DRAW);
	if (auto err = glGetError(); err != GL_NO_ERROR) {
		SDL_LogError(0, "GL Error. Code: %d", err);
	}		

	// Generate IBO and store it in the VAO
	glGenBuffers(1, &mesh.IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(GLuint), indexData, GL_STATIC_DRAW);

	if (auto err = glGetError(); err != GL_NO_ERROR) {
		SDL_LogError(0, "GL Error. Code: %d", err);
	}		

	glBindVertexArray(0);	
	return mesh;
}

Mesh BuildQuad() {
	constexpr GLfloat vertexData[] = { 0.f, 0.f, 1.f, 0.f, 1.f, 1.f, 0.f, 1.f };
	constexpr GLuint  indexData[] = { 0, 1, 2, 3 };
	return BuildMesh(4, 4, vertexData, indexData);
}

Mesh BuildTriangle() {
	constexpr GLfloat vertexData[] = { 0.f, 0.f, 2.f, 0.f, 0.f, 2.f };
	constexpr GLuint  indexData[] = { 0, 1, 2 };
	return BuildMesh(3, 3, vertexData, indexData);
}

} // namespace

struct Graphics::Impl {
	Impl(const SdlWindow& mWindow);

	void          Flush();
	void          EnableClipRect(int x, int y, int width, int height);
	void          DisableClipRect();
	ProgramHandle NewProgram(const char* vs, const char* fs);
	void          Draw(const DrawCall& drawCall);
	void          SetFloat4(int uniform, float x, float y, float z, float w);
	void          SetTexture(int uniform, unsigned texture);
	void          RecompileShaders();
	void          InitGL();

private:
	void ResetState();

public:
	const SdlWindow& mWindow;
	int              mFBWidth = 0;
	int              mFBHeight = 0;
	RectI            mClipRect;
	bool             mClipRectEnabled = false;
	unsigned         mFirstUniform;

	std::vector<Mesh>          mMeshes;
	std::vector<Target>        mTargets;
	std::vector<Batch>         mBatches;
	std::vector<GlProgram>     mPrograms;
	std::vector<ShaderUniform> mShaderUniforms;
};

Graphics::Impl::Impl(const SdlWindow& window)
    : mWindow(window) {
	mFBWidth = window.GetWidth();
	mFBHeight = window.GetHeight();
	mFirstUniform = 0;
}

void Graphics::Impl::InitGL() {
	mMeshes.push_back(BuildQuad());
	mMeshes.push_back(BuildTriangle());
}

void Graphics::Impl::ResetState() {
	mTargets.clear();
	mBatches.clear();
	mShaderUniforms.clear();
	mFirstUniform = 0;
}

void Graphics::Impl::Draw(const DrawCall& drawCall) {
	assert(drawCall.program != nullProgram);
	assert(drawCall.mesh != nullMesh);
	const unsigned meshIdx = static_cast<unsigned>(drawCall.mesh) - 1;

	Batch batch;
	batch.scissorRect = mClipRectEnabled ? mClipRect : RectI { 0, 0, 0, 0 };
	batch.blending = drawCall.blending;
	batch.programIdx = static_cast<unsigned>(drawCall.program) - 1;
	batch.firstUniform = mFirstUniform;
	batch.numUniforms = drawCall.numUniforms + 1;
	batch.sortKey = (static_cast<uint32_t>(mTargets.size() - 1) << 24) | (drawCall.drawOrder << 16) | (meshIdx << 8);
	mBatches.push_back(batch);

	SetTexture(0, drawCall.texture);
	const float* u = drawCall.uniformData;
	for (int i = 0; i < drawCall.numUniforms; ++i) {
		SetFloat4(drawCall.uniforms[i], u[0], u[1], u[2], u[3]);
		u += 4;
	}
	mFirstUniform += (drawCall.numUniforms + 1);
}

void Graphics::Impl::Flush() {
	if (mBatches.empty()) {
		ResetState();
		return;
	}

	glClearColor(0.f, 0.f, 0.f, 1.f);
	glActiveTexture(GL_TEXTURE0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);
	bool blending = false;

	GLuint   currTexture = 0;
	unsigned currProgramIdx = static_cast<unsigned>(-1);
	unsigned currTargetIdx = static_cast<unsigned>(-1);
	unsigned currMeshIdx = static_cast<unsigned>(-1);
	GLsizei  numIndices = 0;
	float    xScale = 0.f;
	float    yScale = 0.f;

	std::sort(std::begin(mBatches), std::end(mBatches), [](const Batch& lhs, const Batch& rhs) { return lhs.sortKey < rhs.sortKey; });

	for (const Batch& batch : mBatches) {
		if (unsigned targetIdx = (batch.sortKey >> 24) & 0xFF; currTargetIdx != targetIdx) {
			currTargetIdx = targetIdx;
			const Target& target = mTargets[targetIdx];
			// To clip space
			xScale = 2.f / target.width;
			yScale = 2.f / target.height;
			glBindFramebuffer(GL_FRAMEBUFFER, target.fbo);
			glViewport(0, 0, target.width, target.height);
			if (target.fbo == 0) {
				glClear(GL_COLOR_BUFFER_BIT);
			}
		}

		if (currProgramIdx != batch.programIdx) {
			currProgramIdx = batch.programIdx;
			glUseProgram(mPrograms[currProgramIdx].GetProgramId());
			// Ortho matrix
			if (auto uniform = mPrograms[currProgramIdx].GetOrthoMatrixUniform(); uniform != -1) {
				glUniform4f(uniform, xScale, yScale, 0.f, 0.f);
			}		
		}

		if (auto err = glGetError(); err != GL_NO_ERROR) {
			SDL_LogError(0, "GL Error. Code: %d", err);
			continue;
		}		

		if (unsigned meshIdx = (batch.sortKey >> 8) & 0xFF; currMeshIdx != meshIdx) {
			const Mesh& mesh = mMeshes[meshIdx];
			glBindVertexArray(mesh.VAO);
			numIndices = mesh.numIndices;
			currMeshIdx = meshIdx;
		}

		if (batch.blending) {
			if (! blending) {
				glEnable(GL_BLEND);
				blending = true;
			}
		}
		else {
			if (blending) {
				glDisable(GL_BLEND);
				blending = false;
			}
		}

		if (batch.scissorRect.left < batch.scissorRect.right) {
			glEnable(GL_SCISSOR_TEST);
			glScissor(batch.scissorRect.left, mFBHeight - (batch.scissorRect.bottom), batch.scissorRect.right - batch.scissorRect.left,
			          batch.scissorRect.bottom - batch.scissorRect.top);
		}
		else {
			glDisable(GL_SCISSOR_TEST);
		}

		// TODO filtering
		for (unsigned ui = 0; ui < batch.numUniforms; ++ui) {
			const ShaderUniform& su = mShaderUniforms[batch.firstUniform + ui];
			if (su.type == ShaderUniformType::float4) {
				glUniform4f(su.uniform, su.fvalue[0], su.fvalue[1], su.fvalue[2], su.fvalue[3]);
			}
			else if (su.type == ShaderUniformType::int4) {
				glUniform4i(su.uniform, su.ivalue[0], su.ivalue[1], su.ivalue[2], su.ivalue[3]);
			}
			else if (su.type == ShaderUniformType::texture) {
				// TODO uniform slot
				// glActiveTexture
				if (su.texture != currTexture) {
					glBindTexture(GL_TEXTURE_2D, su.texture);
					currTexture = su.texture;
				}
			}
		}

		glDrawElements(GL_TRIANGLE_FAN, numIndices, GL_UNSIGNED_INT, NULL);
	}

	// Unbind
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	glUseProgram(0);

	ResetState();
}

void Graphics::Impl::EnableClipRect(int x, int y, int width, int height) {
	mClipRect = { x, y, x + width, y + height };
	mClipRectEnabled = true;
}

void Graphics::Impl::DisableClipRect() {
	mClipRectEnabled = false;
}

void Graphics::Impl::RecompileShaders() {
}

ProgramHandle Graphics::Impl::NewProgram(const char* vs, const char* fs) {
	GlProgram program { vs, fs };
	if (program.Compile()) {
		mPrograms.push_back(std::move(program));
		return static_cast<ProgramHandle>(mPrograms.size());
	}
	else {
		return nullProgram;
	}
}

void Graphics::Impl::SetFloat4(int uniform, float x, float y, float z, float w) {
	mShaderUniforms.resize(mShaderUniforms.size() + 1);
	ShaderUniform& su = mShaderUniforms.back();
	su.type = ShaderUniformType::float4;
	su.uniform = uniform;
	su.fvalue[0] = x;
	su.fvalue[1] = y;
	su.fvalue[2] = z;
	su.fvalue[3] = w;
}

void Graphics::Impl::SetTexture(int uniform, unsigned texture) {
	mShaderUniforms.resize(mShaderUniforms.size() + 1);
	ShaderUniform& su = mShaderUniforms.back();
	su.type = ShaderUniformType::texture;
	su.uniform = uniform;
	su.texture = texture;
}

Graphics::Graphics(const SdlWindow& window)
    : mPimpl { std::make_unique<Impl>(window) } {
}

Graphics::~Graphics() = default;

void Graphics::SetFrameBuffer(const GlFrameBuffer& frameBuffer) {
	mPimpl->mTargets.push_back({ frameBuffer.GetFBO(), frameBuffer.GetWidth(), frameBuffer.GetHeight() });
	mPimpl->mFBWidth = frameBuffer.GetWidth();
	mPimpl->mFBHeight = frameBuffer.GetHeight();
}

void Graphics::SetDefaultFrameBuffer() {
	mPimpl->mTargets.push_back({ 0, mPimpl->mWindow.GetWidth(), mPimpl->mWindow.GetHeight() });
	mPimpl->mFBWidth = mPimpl->mWindow.GetWidth();
	mPimpl->mFBHeight = mPimpl->mWindow.GetHeight();
}

void Graphics::Flush() {
	mPimpl->Flush();
}

int Graphics::GetTargetWidth() const {
	return mPimpl->mFBWidth;
}

int Graphics::GetTargetHeight() const {
	return mPimpl->mFBHeight;
}

ProgramHandle Graphics::NewProgram(const char* vs, const char* fs) {
	return mPimpl->NewProgram(vs, fs);
}

void Graphics::InitGL() {
	mPimpl->InitGL();
}

void Graphics::EnableClipRect(int x, int y, int width, int height) const {
	mPimpl->EnableClipRect(x, y, width, height);
}

void Graphics::DisableClipRect() const {
	mPimpl->DisableClipRect();
}

const GlProgram& Graphics::GetProgram(ProgramHandle program) const {
	assert(program != nullProgram);
	return mPimpl->mPrograms[static_cast<int>(program) - 1];
}

void Graphics::Draw(const DrawCall& drawCall) {
	mPimpl->Draw(drawCall);
}

void Graphics::RecompileShaders() {
	mPimpl->RecompileShaders();
}
} // namespace Wind
