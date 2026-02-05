#include "Font.h"
#include "Texture.h"

#include <algorithm>
#include <cassert>
#include <cstring>

namespace Wind {

Font::Font(const char* name, const TexturePtr& texture, std::vector<Glyph>&& glyphs)
    : mName(name)
    , mTexture(texture)
    , mGlyphs(std::move(glyphs)) {
	uint16_t height = 0;
	for (const Glyph& g : mGlyphs) {
		height = std::max(height, g.height);
	}
	mHeight = height;
	if (const Glyph* g = TryFindGlyph('_'); g) {
		mDefaultGlyph = *g;
	}
	else {
		mDefaultGlyph = Glyph {};
	}
}

const std::string& Font::GetName() const {
	return mName;
}

const Texture& Font::GetTexture() const {
	return *mTexture;
}

const Glyph& Font::FindGlyph(char c) const {
	if (const Glyph* g = TryFindGlyph(c); g) {
		return *g;
	}
	return mDefaultGlyph;
}

int Font::CalculateStringWidth(std::string_view text) const {
	int advance = 0;
	for (char ch : text) {
		const Glyph& g = FindGlyph(ch);
		advance += g.xadvance;
	}
	return advance;
}

int Font::GetHeight() const {
	return mHeight;
}

const Glyph* Font::TryFindGlyph(char c) const {
	auto found = std::lower_bound(std::begin(mGlyphs), std::end(mGlyphs), c);
	if (found == std::end(mGlyphs) || c < *found) {
		return nullptr;
	}
	return &*found;
}


} // namespace Wind
