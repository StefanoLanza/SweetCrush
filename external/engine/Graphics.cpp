#include "Graphics.h"
#include "Config.h"
#include "Gl.h"
#include "GlFrameBuffer.h"
#include "GlProgram.h"
#include "SdlWindow.h"
#include "Texture.h"
#include <SDL3/SDL.h>

#include <algorithm>
#include <cassert>
#include <cstring>
#include <vector>

#define UNIFORM_FILTERING 1

namespace Wind {

namespace {

struct Batch {
	InstanceData instances; // TODO compress
	uint32_t     sortKey;
	uint16_t     programIdx;
	uint16_t     pipelineIdx;
	uint16_t     firstUniform;
	uint8_t      numUniforms;
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

struct TextureAndSampler {
	unsigned texture;
	unsigned sampler;
};

struct ShaderUniform {
	ShaderUniformType type;
	GLint             uniform;
	union {
		float             fvalue[4];
		int               ivalue[4];
		unsigned          uvalue[4];
		TextureAndSampler texture;
	};
};

Mesh BuildMesh(GLuint numVertices, GLsizei numIndices, const GLfloat vertexData[], const GLushort indexData[]) {
	constexpr GLuint numComponents = 2; // x,y for position
	Mesh             mesh {};
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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(GLushort), indexData, GL_STATIC_DRAW);

	if (auto err = glGetError(); err != GL_NO_ERROR) {
		SDL_LogError(0, "GL Error. Code: %d", err);
	}

	glBindVertexArray(0);
	return mesh;
}

Mesh BuildQuad() {
	constexpr GLfloat  vertexData[] = { 0.f, 0.f, 1.f, 0.f, 1.f, 1.f, 0.f, 1.f };
	constexpr GLushort indexData[] = { 0, 1, 2, 3 };
	return BuildMesh(4, 4, vertexData, indexData);
}

Mesh BuildTriangle() {
	constexpr GLfloat  vertexData[] = { 0.f, 0.f, 1.f, 0.f, 0.f, 1.f };
	constexpr GLushort indexData[] = { 0, 1, 2 };
	return BuildMesh(3, 3, vertexData, indexData);
}

} // namespace

struct Graphics::Impl {
	explicit Impl(const SdlWindow& mWindow);

	void           Flush();
	void           SetPipeline(PipelineHandle pipeline);
	ProgramHandle  NewProgram(const char* vs, const char* fs, const char* defines);
	PipelineHandle NewPipeline(const PipelineState& pipelineState);
	TexturePtr     LoadTexture(std::string_view fileName, TextureInfo texInfo);
	void           Draw(const DrawCall& drawCall);
	void           SetFloat4(int uniform, float x, float y, float z, float w);
	void           SetTexture(int uniform, unsigned texture, unsigned sampler);
	void           RecompileShaders();
	void           InitGL();
	InstanceData   AllocInstances(unsigned count, unsigned sizePerInstance, GLint location);
	void           ResetState();

public:
	const SdlWindow&           mWindow;
	int                        mFBWidth = 0;
	int                        mFBHeight = 0;
	unsigned                   mFirstUniform;
	PipelineHandle             mDefaultPipeline;
	PipelineHandle             mCurrPipeline;
	std::vector<Mesh>          mMeshes;
	std::vector<Target>        mTargets;
	std::vector<Batch>         mBatches;
	std::vector<GlProgram>     mPrograms;
	std::vector<ShaderUniform> mShaderUniforms;
	std::vector<PipelineState> mPipelineStates;
	std::vector<TexturePtr>    mTextures;
	std::vector<char>          mInstanceBuffer;
	unsigned                   mInstanceBufferOffs;
	GLuint                     mInstanceVBO;
	unsigned                   mFrameCount;
	bool                       mFrameBegun;
};

Graphics::Impl::Impl(const SdlWindow& window)
    : mWindow(window) {
	mFBWidth = window.GetWidth();
	mFBHeight = window.GetHeight();
	mFirstUniform = 0;
	mFrameCount = 0;
	mFrameBegun = false;

	PipelineState defaultPipeline;
	mDefaultPipeline = NewPipeline(defaultPipeline);
	mCurrPipeline = mDefaultPipeline;

	glGenBuffers(1, &mInstanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mInstanceVBO);
	glBufferData(GL_ARRAY_BUFFER, instanceDataBufferSize, nullptr, GL_DYNAMIC_DRAW);
	mInstanceBuffer.resize(instanceDataBufferSize);
	mInstanceBufferOffs = 0;
}

void Graphics::Impl::InitGL() {
	mMeshes.push_back(BuildQuad());
	mMeshes.push_back(BuildTriangle());
}

InstanceData Graphics::Impl::AllocInstances(unsigned count, unsigned stride, GLint location) {
	unsigned size = count * stride;
	if (mInstanceBufferOffs + size > mInstanceBuffer.size()) {
		SDL_LogError(0, "Cannot allocate instance data (count: %d stride: %d)", count, stride);
		return { nullptr, 0, 0, 0 };
	}
	unsigned offs = mInstanceBufferOffs;
	mInstanceBufferOffs += size;
	return { mInstanceBuffer.data() + offs, stride, count, location };
}

void Graphics::Impl::ResetState() {
	mTargets.clear();
	mBatches.clear();
	mShaderUniforms.clear();
	mFirstUniform = 0;
	mCurrPipeline = mDefaultPipeline;
	mInstanceBufferOffs = 0;
}

void Graphics::Impl::Draw(const DrawCall& drawCall) {
	assert(drawCall.program != nullProgram);
	assert(drawCall.mesh != nullMesh);
	assert(mCurrPipeline != nullPipeline);
	assert(drawCall.drawOrder < (1u << 12));
	assert(drawCall.numUniforms + drawCall.numTextures < 256);
	const uint32_t meshIdx = static_cast<unsigned>(drawCall.mesh) - 1;
	const uint32_t targetIdx = static_cast<uint32_t>(mTargets.size() - 1);
	assert(targetIdx < 16);
	// const uint32_t programIdx = static_cast<uint32_t>(drawCall.program) - 1;

	Batch batch;
	batch.instances = drawCall.instances;
	batch.sortKey = (targetIdx << 28) | (drawCall.drawOrder << 16) | (drawCall.sortKey << 8) | (meshIdx << 0);
	batch.pipelineIdx = static_cast<uint16_t>(mCurrPipeline) - 1;
	batch.programIdx = static_cast<uint16_t>(drawCall.program) - 1;
	batch.firstUniform = static_cast<uint16_t>(mFirstUniform);
	batch.numUniforms = static_cast<uint8_t>(drawCall.numUniforms + drawCall.numTextures);
	mBatches.push_back(batch);

	mShaderUniforms.reserve(mShaderUniforms.size() + batch.numUniforms);
	if (drawCall.samplers) {
		for (int i = 0; i < drawCall.numTextures; ++i) {
			SetTexture(i, drawCall.textures[i], drawCall.samplers[i]);
		}
	}
	else {
		for (int i = 0; i < drawCall.numTextures; ++i) {
			SetTexture(i, drawCall.textures[i], 0 /*default */);
		}
	}
	const float* u = static_cast<const float*>(drawCall.uniformData);
	for (int i = 0; i < drawCall.numUniforms; ++i) {
		SetFloat4(drawCall.uniforms[i], u[0], u[1], u[2], u[3]);
		u += 4;
	}
	mFirstUniform += batch.numUniforms;
}

void Graphics::Impl::Flush() {
	if (mBatches.empty()) {
		ResetState();
		return;
	}

	glActiveTexture(GL_TEXTURE0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);

	if (auto err = glGetError(); err != GL_NO_ERROR) {
		SDL_LogError(0, "GL Error. Code: %d", err);
		ResetState();
		return;
	}

	GLuint   currTexture[16] {};
	unsigned currProgramIdx = static_cast<unsigned>(-1);
	unsigned currTargetIdx = static_cast<unsigned>(-1);
	unsigned currMeshIdx = static_cast<unsigned>(-1);
	GLsizei  numIndices = 0;
	float    xScale = 0.f;
	float    yScale = 0.f;

	// State filters
	int blendEnabled = -1;
	int depthEnabled = -1;
	int depthWriteEnabled = -1;
	int scissorTest = -1;
	int pipelineIdx = -1;

	std::stable_sort(std::begin(mBatches), std::end(mBatches), [](const Batch& lhs, const Batch& rhs) { return lhs.sortKey < rhs.sortKey; });

	uint32_t cachedUniformHash[16];
	uint32_t cachedUniformValue[16][4];

	for (const Batch& batch : mBatches) {
		if (unsigned targetIdx = (batch.sortKey >> 28) & 0xF; currTargetIdx != targetIdx) {
			// Change target
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
			// Uniforms are per program, reset cached values
			std::memset(cachedUniformHash, 0, sizeof cachedUniformHash);
			std::memset(cachedUniformValue, 0, sizeof cachedUniformValue);
			std::memset(currTexture, ~0, sizeof currTexture);
			// Ortho matrix
			if (auto uniform = mPrograms[currProgramIdx].GetOrthoMatrixUniform(); uniform != -1) {
				glUniform4f(uniform, xScale, yScale, 0.f, 0.f);
			}
		}

		if (unsigned meshIdx = (batch.sortKey >> 0) & 0xFF; currMeshIdx != meshIdx) {
			const Mesh& mesh = mMeshes[meshIdx];
			glBindVertexArray(mesh.VAO);
			numIndices = mesh.numIndices;
			currMeshIdx = meshIdx;
		}

		if (batch.pipelineIdx != pipelineIdx) {
			pipelineIdx = batch.pipelineIdx;
			const PipelineState& ps = mPipelineStates[batch.pipelineIdx];
			if (ps.mDepthEnabled) {
				if (depthEnabled != 1) {
					glEnable(GL_DEPTH_TEST);
					glDepthFunc(GL_LEQUAL);
					depthEnabled = 1;
				}
			}
			else {
				if (depthEnabled != 0) {
					glDisable(GL_DEPTH_TEST);
					depthEnabled = 0;
				}
			}
			if (ps.mDepthWriteEnabled) {
				if (depthWriteEnabled != 1) {
					glDepthMask(GL_TRUE);
					depthWriteEnabled = 1;
				}
			}
			else {
				if (depthWriteEnabled != 0) {
					glDepthMask(GL_FALSE);
					depthWriteEnabled = 0;
				}
			}

			if (ps.mBlending) {
				if (blendEnabled != 1) {
					glEnable(GL_BLEND);
					blendEnabled = 1;
				}
			}
			else {
				if (blendEnabled != 0) {
					glDisable(GL_BLEND);
					blendEnabled = 0;
				}
			}

			if (ps.mScissorTestEnabled) {
				if (scissorTest != 1) {
					glEnable(GL_SCISSOR_TEST);
					scissorTest = 1;
				}
				glScissor(ps.mScissorRect.left, mFBHeight - (ps.mScissorRect.bottom), ps.mScissorRect.right - ps.mScissorRect.left,
				          ps.mScissorRect.bottom - ps.mScissorRect.top);
			}
			else {
				if (scissorTest != 0) {
					glDisable(GL_SCISSOR_TEST);
					scissorTest = 0;
				}
			}
		}

		if (auto err = glGetError(); err != GL_NO_ERROR) {
			SDL_LogError(0, "GL Error. Code: %d", err);
			continue;
		}

		GLuint textureUnit = 0;
		for (unsigned ui = 0; ui < batch.numUniforms; ++ui) {
			const ShaderUniform& su = mShaderUniforms[batch.firstUniform + ui];
			assert(su.uniform >= 0);
			assert(su.uniform < (GLint)std::size(cachedUniformHash));
			if (su.type == ShaderUniformType::float4) {
#if UNIFORM_FILTERING
				uint32_t hash = HashUint4(su.uvalue);
				if (cachedUniformHash[su.uniform] != hash || memcmp(cachedUniformValue[su.uniform], su.fvalue, 16)) {
					memcpy(cachedUniformValue[su.uniform], su.fvalue, 16);
					cachedUniformHash[su.uniform] = hash;
					glUniform4f(su.uniform, su.fvalue[0], su.fvalue[1], su.fvalue[2], su.fvalue[3]);
				}
#else
				glUniform4f(su.uniform, su.fvalue[0], su.fvalue[1], su.fvalue[2], su.fvalue[3]);
#endif
			}
			else if (su.type == ShaderUniformType::int4) {
#if UNIFORM_FILTERING
				uint32_t hash = HashUint4(su.uvalue);
				if (cachedUniformHash[su.uniform] != hash || memcmp(cachedUniformValue[su.uniform], su.ivalue, 16)) {
					memcpy(cachedUniformValue[su.uniform], su.ivalue, 16);
					cachedUniformHash[su.uniform] = hash;
					glUniform4i(su.uniform, su.ivalue[0], su.ivalue[1], su.ivalue[2], su.ivalue[3]);
				}
#else
				glUniform4i(su.uniform, su.ivalue[0], su.ivalue[1], su.ivalue[2], su.ivalue[3]);

#endif
			}
			else if (su.type == ShaderUniformType::texture) {
				assert(textureUnit < (GLint)std::size(currTexture));
				if (su.texture.texture != currTexture[textureUnit]) {
					glActiveTexture(GL_TEXTURE0 + textureUnit);
					glBindTexture(GL_TEXTURE_2D, su.texture.texture);
					glBindSampler(textureUnit, su.texture.sampler);
					// glUniform1i(su.texture, textureUnit);
					currTexture[textureUnit] = su.texture.texture;
				}
				if (batch.firstUniform)
					++textureUnit;
			}
		}

		if (batch.instances.data) {
			// Orphan the previous storage
			glBindBuffer(GL_ARRAY_BUFFER, mInstanceVBO);
			glBufferData(GL_ARRAY_BUFFER, batch.instances.stride * batch.instances.count, nullptr, GL_DYNAMIC_DRAW);
			glBufferSubData(GL_ARRAY_BUFFER, 0, batch.instances.stride * batch.instances.count, batch.instances.data);

			size_t offset = 0;
			for (unsigned i = 0; i < batch.instances.stride / 16; ++i) {
				glEnableVertexAttribArray(batch.instances.location + i);
				glVertexAttribPointer(batch.instances.location + i, 4, GL_FLOAT, GL_FALSE, batch.instances.stride,
				                      reinterpret_cast<void*>(offset + i * 16));
				glVertexAttribDivisor(batch.instances.location + i, 1);
			}

			glDrawElementsInstanced(GL_TRIANGLE_FAN, numIndices, GL_UNSIGNED_SHORT, nullptr, batch.instances.count);
		}
		else {
			glDrawElements(GL_TRIANGLE_FAN, numIndices, GL_UNSIGNED_SHORT, nullptr);
		}
	}

	// Unbind
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	glUseProgram(0);
	glBindSampler(0, 0);

	ResetState();
}

void Graphics::Impl::SetPipeline(PipelineHandle pipelineHandle) {
	mCurrPipeline = pipelineHandle;
}

void Graphics::Impl::RecompileShaders() {
}

ProgramHandle Graphics::Impl::NewProgram(const char* vs, const char* fs, const char* defines) {
	unsigned int idx = 1;
	for (const auto& p : mPrograms) {
		if (p.IsEqual(vs, fs, defines)) {
			return static_cast<ProgramHandle>(idx);
		}
		++idx;
	}
	GlProgram program { vs, fs, defines };
	if (program.Compile()) {
		mPrograms.push_back(std::move(program));
		return static_cast<ProgramHandle>(mPrograms.size());
	}
	else {
		return nullProgram;
	}
}

PipelineHandle Graphics::Impl::NewPipeline(const PipelineState& pipelineState) {
	unsigned int idx = 1;
	for (const auto& ps : mPipelineStates) {
		if (! std::memcmp(&pipelineState, &ps, sizeof pipelineState)) {
			return static_cast<PipelineHandle>(idx);
		}
		++idx;
	}
	mPipelineStates.push_back(pipelineState);
	return static_cast<PipelineHandle>(mPipelineStates.size());
}

TexturePtr Graphics::Impl::LoadTexture(std::string_view fileName, TextureInfo texInfo) {
	try {
		for (auto& b : mTextures) {
			if (b->GetFileName() == fileName) {
				return b;
			}
		}
		char path[260];
		snprintf(path, sizeof(path), "%s%s", ASSETS_FOLDER, fileName.data());
		mTextures.emplace_back(std::make_unique<Texture>(fileName, path, texInfo));
		return mTextures.back();
	}
	catch (const std::exception& e) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", e.what());
		return mTextures.empty() ? nullptr : mTextures[0]; // placeholder
	}
}

void Graphics::Impl::SetFloat4(int uniform, float x, float y, float z, float w) {
	ShaderUniform su;
	su.type = ShaderUniformType::float4;
	su.uniform = uniform;
	su.fvalue[0] = x;
	su.fvalue[1] = y;
	su.fvalue[2] = z;
	su.fvalue[3] = w;
	mShaderUniforms.push_back(su);
}

void Graphics::Impl::SetTexture(int uniform, unsigned texture, unsigned sampler) {
	ShaderUniform su;
	su.type = ShaderUniformType::texture;
	su.uniform = uniform;
	su.texture.texture = texture;
	su.texture.sampler = sampler;
	mShaderUniforms.push_back(su);
}

void PipelineState::EnableDepth() {
	mDepthEnabled = true;
}

void PipelineState::DisableDepth() {
	mDepthEnabled = false;
}

void PipelineState::EnableScissorTest(int x, int y, int width, int height) {
	mScissorRect = { x, y, x + width, y + height };
	mScissorTestEnabled = true;
}

void PipelineState::DisableScissorTest() {
	mScissorTestEnabled = false;
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

void Graphics::ClearDefaultFrameBuffer(float r, float g, float b, float a) {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, mPimpl->mWindow.GetWidth(), mPimpl->mWindow.GetHeight());
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT);
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

ProgramHandle Graphics::NewProgram(const char* vs, const char* fs, const char* defines) {
	return mPimpl->NewProgram(vs, fs, defines);
}

PipelineHandle Graphics::NewPipeline(const PipelineState& pipelineState) {
	return mPimpl->NewPipeline(pipelineState);
}

TexturePtr Graphics::LoadTexture(std::string_view fileName, TextureInfo texInfo) {
	return mPimpl->LoadTexture(fileName, texInfo);
}

void Graphics::InitGL() {
	mPimpl->InitGL();
}

void Graphics::BeginFrame() {
	assert(! mPimpl->mFrameBegun);
	mPimpl->mFrameBegun = true;
	// Set defaults
	glDisable(GL_BLEND);
	glDisable(GL_SCISSOR_TEST);
}

void Graphics::EndFrame() {
	assert(mPimpl->mFrameBegun);
	++mPimpl->mFrameCount;
	mPimpl->ResetState();
	mPimpl->mFrameBegun = false;
}

void Graphics::SetPipeline(PipelineHandle pipeline) {
	mPimpl->SetPipeline(pipeline);
}

void Graphics::SetDefaultPipeline() {
	mPimpl->mCurrPipeline = mPimpl->mDefaultPipeline;
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

InstanceData Graphics::AllocInstances(unsigned count, unsigned stride, int location) {
	assert(location >= 0);
	return mPimpl->AllocInstances(count, stride, location);
}

} // namespace Wind
