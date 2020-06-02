#include "Graphics.h"
#include "Config.h"
#include "Gl.h"
#include "GlFrameBuffer.h"
#include "GlProgram.h"
#include "SdlSurface.h"
#include "SdlWindow.h"
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
	GLuint  VBO;
	GLuint  IBO;
	GLint   numComponents;
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

Mesh BuildQuad() {
	Mesh          quad {};
	const GLfloat vertexData[] = { 0.f, 0.f, 1.f, 0.f, 1.f, 1.f, 0.f, 1.f };
	const GLuint  indexData[] = { 0, 1, 2, 3 };

	glGenBuffers(1, &quad.VBO);
	glBindBuffer(GL_ARRAY_BUFFER, quad.VBO);
	glBufferData(GL_ARRAY_BUFFER, 2 * 4 * sizeof(GLfloat), vertexData, GL_STATIC_DRAW);

	glGenBuffers(1, &quad.IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad.IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), indexData, GL_STATIC_DRAW);

	quad.numIndices = 4;
	quad.numComponents = 2;
	return quad;
}

Mesh BuildTriangle() {
	Mesh          mesh {};
	const GLfloat vertexData[] = { 0.f, 0.f, 2.f, 0.f, 0.f, 2.f };
	const GLuint  indexData[] = { 0, 1, 2 };

	glGenBuffers(1, &mesh.VBO);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
	glBufferData(GL_ARRAY_BUFFER, 2 * 3 * sizeof(GLfloat), vertexData, GL_STATIC_DRAW);

	glGenBuffers(1, &mesh.IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(GLuint), indexData, GL_STATIC_DRAW);

	mesh.numIndices = 3;
	mesh.numComponents = 2;
	return mesh;
}

} // namespace

struct Graphics::Impl {
	Impl(const SdlWindow& mWindow);

	void          Flush();
	void          EnableClipRect(int x, int y, int width, int height);
	void          DisableClipRect();
	void          SetBlending(bool enabled);
	void          SetProgram(ProgramHandle program);
	void          SetMesh(MeshHandle mesh);
	ProgramHandle NewProgram(const char* vs, const char* fs);
	void          SetDefaultProgram();
	void          BeginBatch(unsigned int drawOrder);
	void          EndBatch();
	void          SetFloat4(int uniform, float x, float y, float z, float w);
	void          SetTexture(int uniform, unsigned texture);
	void          Draw(const DrawCall& drawCall);
	void          RecompileShaders();
	void          InitGL();

private:
	void ResetState();

public:
	const SdlWindow& mWindow;
	int              mFBWidth = 0;
	int              mFBHeight = 0;
	ProgramHandle    mCurrProgram;
	unsigned         mMeshIdx;
	RectI            mClipRect;
	bool             mClipRectEnabled = false;
	bool             mBlending = true;
	unsigned         mFirstUniform;
	unsigned         mUniformCount;
	unsigned         mDrawOrder;
	bool             mBeginBatch;

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
	mCurrProgram = NewProgram(SHADERS_FOLDER "quad.vs", SHADERS_FOLDER "quad.fs");
	mFirstUniform = 0;
	mUniformCount = 0;
	mBeginBatch = false;
	mMeshIdx = 0;
}

void Graphics::Impl::InitGL() {
	glClearColor(0.0, 0.0, 0.0, 1.0);
	mMeshes.push_back(BuildQuad());
	mMeshes.push_back(BuildTriangle());
}

void Graphics::Impl::ResetState() {
	mTargets.clear();
	mBatches.clear();
	mShaderUniforms.clear();
	SetDefaultProgram();
	mFirstUniform = 0;
	mUniformCount = 0;
	SetMesh(quadMesh);
}

void Graphics::Impl::BeginBatch(unsigned int drawOrder) {
	assert(! mBeginBatch);
	mBeginBatch = true;
	mUniformCount = 0;
	mDrawOrder = drawOrder;
}

void Graphics::Impl::EndBatch() {
	assert(mBeginBatch);
	Batch batch;
	batch.scissorRect = mClipRectEnabled ? mClipRect : RectI { 0, 0, 0, 0 };
	batch.blending = mBlending;
	batch.programIdx = static_cast<unsigned>(mCurrProgram) - 1;
	batch.firstUniform = mFirstUniform;
	batch.numUniforms = mUniformCount;
	batch.sortKey = (static_cast<uint32_t>(mTargets.size() - 1) << 24) | (mDrawOrder << 16) | (mMeshIdx << 8);
	mBatches.push_back(batch);
	mBeginBatch = false;
	mFirstUniform += mUniformCount;
}

void Graphics::Impl::Flush() {
	if (mBatches.empty()) {
		ResetState();
		return;
	}

	glActiveTexture(GL_TEXTURE0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);
	glEnableVertexAttribArray(0);
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
		}

		if (currProgramIdx != batch.programIdx) {
			currProgramIdx = batch.programIdx;
			glUseProgram(mPrograms[currProgramIdx].GetProgramId());
			// Ortho matrix
			glUniform4f(mPrograms[currProgramIdx].GetOrthoMatrixUniform(), xScale, yScale, 0.f, 0.f);
		}

		if (unsigned meshIdx = (batch.sortKey >> 8) & 0xFF; currMeshIdx != meshIdx) {
			const Mesh& mesh = mMeshes[meshIdx];
			glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.IBO);
			glVertexAttribPointer(0, mesh.numComponents, GL_FLOAT, GL_FALSE, mesh.numComponents * sizeof(GLfloat), NULL);
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

void Graphics::Impl::SetBlending(bool enabled) {
	mBlending = enabled;
}

void Graphics::Impl::SetProgram(ProgramHandle program) {
	mCurrProgram = program;
}

void Graphics::Impl::SetMesh(MeshHandle mesh) {
	assert(mesh != nullMesh);
	mMeshIdx = static_cast<unsigned>(mesh) - 1;
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

void Graphics::Impl::SetDefaultProgram() {
	mCurrProgram = static_cast<ProgramHandle>(1);
}

void Graphics::Impl::SetFloat4(int uniform, float x, float y, float z, float w) {
	assert(mBeginBatch);
	mShaderUniforms.resize(mShaderUniforms.size() + 1);
	ShaderUniform& su = mShaderUniforms.back();
	su.type = ShaderUniformType::float4;
	su.uniform = uniform;
	su.fvalue[0] = x;
	su.fvalue[1] = y;
	su.fvalue[2] = z;
	su.fvalue[3] = w;
	++mUniformCount;
}

void Graphics::Impl::SetTexture(int uniform, unsigned texture) {
	assert(mBeginBatch);
	mShaderUniforms.resize(mShaderUniforms.size() + 1);
	ShaderUniform& su = mShaderUniforms.back();
	su.type = ShaderUniformType::texture;
	su.uniform = uniform;
	su.texture = texture;
	++mUniformCount;
}

void Graphics::Impl::Draw(const DrawCall& drawCall) {
	BeginBatch(drawCall.drawOrder);
	SetProgram(drawCall.program);
	SetMesh(drawCall.mesh);
	SetTexture(0, drawCall.texture);
	SetBlending(drawCall.blending);
	DisableClipRect(); // FIXME
	const float* u = drawCall.uniformData;
	for (int i = 0; i < drawCall.numUniforms; ++i) {
		SetFloat4(drawCall.uniforms[i], u[0], u[1], u[2], u[3]);
		u += 4;
	}
	EndBatch();
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
