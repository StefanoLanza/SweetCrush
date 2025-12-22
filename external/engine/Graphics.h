#pragma once

#include "Color.h"
#include "Maths.h"

#include <memory>
#include <string_view>

namespace Wind {

class SdlSurface;
class SdlWindow;
class GlFrameBuffer;
class GlProgram;

enum class ProgramHandle : uint32_t;
constexpr ProgramHandle nullProgram = static_cast<ProgramHandle>(0);

enum class MeshHandle : uint32_t;
constexpr MeshHandle nullMesh = static_cast<MeshHandle>(0);
// Predefines meshes
constexpr MeshHandle quadMesh = static_cast<MeshHandle>(1);
constexpr MeshHandle triangleMesh = static_cast<MeshHandle>(2);

struct InstanceData {
	void*    data;
	uint32_t stride; // bytes
	uint32_t count;
};

struct DrawCall {
	const int*      uniforms = nullptr;
	const void*     uniformData = nullptr;
	int             numUniforms = 0;
	const unsigned* textures;
	int             numTextures = 0;
	ProgramHandle   program = nullProgram;
	MeshHandle      mesh = nullMesh;
	uint32_t        drawOrder = 0;
	uint32_t        sortKey = 0;
	InstanceData    instances {};
};

class PipelineState {
public:
	void EnableDepth();
	void DisableDepth();
	void EnableScissorTest(int x, int y, int width, int height);
	void DisableScissorTest();

public:
	RectI mScissorRect {};
	bool  mDepthEnabled = false;
	bool  mDepthWriteEnabled = false;
	bool  mScissorTestEnabled = false;
	bool  mBlending = false;

	friend class Graphics;
};

enum class PipelineHandle : uint32_t;
constexpr PipelineHandle nullPipeline = static_cast<PipelineHandle>(0);

class Graphics final {
public:
	explicit Graphics(const SdlWindow& window);
	~Graphics();

	int              GetTargetWidth() const;
	int              GetTargetHeight() const;
	ProgramHandle    NewProgram(const char* vs, const char* fs, const char* defines = "");
	PipelineHandle   NewPipeline(const PipelineState& pipelineState);
	void             SetFrameBuffer(const GlFrameBuffer& frameBuffer);
	void             SetDefaultFrameBuffer();
	void             ClearDefaultFrameBuffer(float r, float g, float b, float a);
	void             Flush();
	void             InitGL();
	void             BeginFrame();
	void             EndFrame();
	void             SetPipeline(PipelineHandle pipeline);
	void             SetDefaultPipeline();
	const GlProgram& GetProgram(ProgramHandle program) const;
	void             Draw(const DrawCall& drawCall);
	void             RecompileShaders();
	const Stats&     GetFrameTime() const;
	InstanceData     AllocInstances(unsigned count, unsigned stride);

private:
	struct Impl;
	std::unique_ptr<Impl> mPimpl;
};

} // namespace Wind
