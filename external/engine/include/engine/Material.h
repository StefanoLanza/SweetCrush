#pragma once

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
	union {
		Vec4        fvalue;
		int         ivalue[4];
		unsigned    uvalue[4];
		const char* texturePath;
	};
};

class MaterialInfo {
public:
	void                      AddFloat4(Vec4 v4);
	void                      AddTexture(const char* texturePath);
	Span<const MaterialParam> Params() const;

private:
	MaterialParam mParams[8];
	int           mNumParams = 0;
};

} // namespace Wind
