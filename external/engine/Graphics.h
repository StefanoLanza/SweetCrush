#pragma once

#include "Color.h"
#include "Math.h"
#include <memory>

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

struct DrawCall {
	ProgramHandle program = nullProgram;
	MeshHandle    mesh = nullMesh;
	unsigned      texture = 0;
	unsigned      drawOrder = 0;
	bool          blending = false;
	const int*    uniforms = nullptr;
	const float*  uniformData = nullptr;
	int           numUniforms = 0;
};

class Graphics {
public:
	Graphics(const SdlWindow& window);
	~Graphics();

	int              GetTargetWidth() const;
	int              GetTargetHeight() const;
	ProgramHandle    NewProgram(const char* vs, const char* fs);
	void             SetFrameBuffer(const GlFrameBuffer& frameBuffer);
	void             SetDefaultFrameBuffer();
	void             Flush();
	void             InitGL();
	void             EnableClipRect(int x, int y, int width, int height) const;
	void             DisableClipRect() const;
	const GlProgram& GetProgram(ProgramHandle program) const;
	void             Draw(const DrawCall& drawCall);
	void             RecompileShaders();

private:
	struct Impl;
	std::unique_ptr<Impl> mPimpl;
};

} // namespace Wind
