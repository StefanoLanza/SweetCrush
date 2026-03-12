#include "Material.h"
#include "GlProgram.h"

#include <SDL3/SDL_log.h>
#include <cassert>
#include <cstdlib>
#include <iterator> // std::size

namespace Wind {

const char* MaterialInfo::GetVS() const {
	return mVS;
}

const char* MaterialInfo::GetFS() const {
	return mFS;
}

void MaterialInfo::SetShader(const char* vs, const char* fs) {
	assert(vs);
	assert(fs);
	mVS = vs;
	mFS = fs;
}

void MaterialInfo::AddFloat4(const char* name, Vec4 v4) {
	assert(mNumParams < (int)std::size(mParams));
	assert(name);
	mParams[mNumParams].type = MaterialParamType::float4;
	mParams[mNumParams].name = name;
	mParams[mNumParams].fvalue = v4;
	++mNumParams;
}

void MaterialInfo::AddTexture(const char* name, const char* texturePath) {
	assert(mNumParams < (int)std::size(mParams));
	assert(name);
	mParams[mNumParams].type = MaterialParamType::texture;
	mParams[mNumParams].name = name;
	mParams[mNumParams].texturePath = texturePath; // FIXME store string ?
	++mNumParams;
}

Span<const MaterialParam> MaterialInfo::Params() const {
	return { mParams, (size_t)mNumParams };
}

Material MakeMaterial(const MaterialInfo& info, Graphics& graphics) {
	Material material;
	material.mProgram = graphics.NewProgram(info.GetVS(), info.GetFS());
	if (material.mProgram == nullProgram) {
		return material; // TODO null
	}
	int              ui = 0;
	int              ti = 0;
	const GlProgram& program = graphics.GetProgram(material.mProgram);

	const int uniformCount = program.GetUniformCount();
	for (int i = 0; i < uniformCount; ++i) {
		const GlUniform u = program.QueryUniform(i);
		if (u.type == GL_FLOAT_VEC4) {
			for (const auto& prm : info.Params()) {
				if (! strcmp(u.name, prm.name)) {
					if (prm.type == MaterialParamType::float4) {
						assert(ui < (int)material.maxUniforms);
						material.mUniformLoc[ui] = u.location;
						std::memcpy(material.mUniformData + 4 * ui, &prm.fvalue, sizeof prm.fvalue);
						++ui;
					}
					else {
						SDL_LogError(0, "Type of material parameter differs from that of shader uniform %s", u.name);
					}
					break;
				}
			}
		}
		else if (u.type == GL_SAMPLER_2D) {
			for (const auto& prm : info.Params()) {
				if (! strcmp(u.name, prm.name)) {
					if (prm.type == MaterialParamType::texture) {
						assert(ti < (int)material.maxTextures);
						auto texture = graphics.LoadTexture(prm.texturePath);
						if (texture) {
							material.mTextureIDs[ti] = texture->GetGLId();
						}
						else {
							SDL_LogError(0, "Failed to load texture %s for material uniform %s", prm.texturePath, u.name);
						}
						// else fallback ??
						++ti;
					}
					else {
						SDL_LogError(0, "Type of material parameter differs from that of shader uniform %s", u.name);
					}
					break;
				}
			}
		}
		else {
			SDL_LogWarn(0, "Unsupported type (%d) for uniform %s", u.type, u.name);
		}
	}

	return material;
}

} // namespace Wind
