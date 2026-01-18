#include "Material.h"

#include <iterator>
#include <cassert>

namespace Wind {

void MaterialInfo::AddFloat4(Vec4 v4) {
	assert(mNumParams < (int)std::size(mParams));
	mParams[mNumParams].type = MaterialParamType::float4;
	mParams[mNumParams].fvalue = v4;
	++mNumParams;
}

void MaterialInfo::AddTexture(const char* texturePath) {
	assert(mNumParams < (int)std::size(mParams));
	mParams[mNumParams].type = MaterialParamType::texture;
	mParams[mNumParams].texturePath = texturePath; //FIXME store string ?
	++mNumParams;
}

Span<const MaterialParam> MaterialInfo::Params() const {
	return mParams;
}

} // namespace Wind
