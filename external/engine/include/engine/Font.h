#pragma once

#include "Texture.h"
#include <cstdint>
#include <vector>

namespace Wind {

struct Glyph {
#pragma pack(push)
#pragma pack(1)
	uint32_t id;
	uint16_t x;
	uint16_t y;
	uint16_t width;
	uint16_t height;
	int16_t  xoffset;
	int16_t  yoffset;
	int16_t  xadvance;
	uint8_t  page;
	uint8_t  chnl;
#pragma pack(pop)
};

inline bool operator<(const Glyph& glyph, uint32_t id) {
	return glyph.id < id;
}
inline bool operator<(uint32_t id, const Glyph& glyph) {
	return id < glyph.id;
}
inline bool operator<(const Glyph& lhs, const Glyph& rhs) {
	return lhs.id < rhs.id;
}

class Font final {
public:
	Font(const char* name, const char* textureFile, const char* texturePath, std::vector<Glyph>&& glyphs);
	const std::string& GetName() const;
	const Texture&  GetSurface() const;
	const Glyph&       FindGlyph(char c) const;
	int                CalculateStringWidth(std::string_view text) const;
	int                GetHeight() const;

private:
	const Glyph* TryFindGlyph(char c) const;

private:
	std::string        mName;
	Texture         mSurface;
	std::vector<Glyph> mGlyphs;
	Glyph              mDefaultGlyph;
	int                mHeight;
};

std::vector<Glyph> LoadGlyphs(const char* fontFile);

} // namespace Wind
