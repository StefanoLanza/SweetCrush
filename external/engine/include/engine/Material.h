#pragma once

#include "Graphics.h"
#include "Maths.h"
#include "Span.h"

namespace Wind {

enum class MaterialParamType {
	float4,
	int4,
	texture,
};

struct MaterialParam {
	MaterialParamType type;
	const char*       name;
	union {
		Vec4        fvalue;
		int         ivalue[4];
		unsigned    uvalue[4];
		const char* texturePath;
	};
};

class MaterialInfo {
public:
	const char*               GetVS() const;
	const char*               GetFS() const;
	void                      SetShader(const char* vs, const char* fs);
	void                      AddFloat4(const char* name, Vec4 v4);
	void                      AddTexture(const char* name, const char* texturePath); // TODO Sampler
	Span<const MaterialParam> Params() const;

private:
	const char*   mVS = nullptr;
	const char*   mFS = nullptr;
	MaterialParam mParams[8];
	int           mNumParams = 0;

	friend class Graphics;
};

struct Material {
	static constexpr int maxUniforms = 8;
	static constexpr int maxTextures = 4;
	ProgramHandle        mProgram = nullProgram;
	GLint                mUniformLoc[maxUniforms];
	GLint                mTextureLoc[maxTextures];
	float                mUniformData[maxUniforms * 4];
	GLuint               mTextureIDs[maxTextures];
};

Material MakeMaterial(const MaterialInfo& info, Graphics& graphics);

} // namespace Wind
