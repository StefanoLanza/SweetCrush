#include "Font.h"
#include "Texture.h"
#include <SDL3/SDL.h>
#include <algorithm>
#include <cassert>
#include <stdexcept>
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

std::vector<Glyph> LoadGlyphs(const char* fileName) {
	SDL_IOStream* f = SDL_IOFromFile(fileName, "rb");
	if (! f) {
		throw std::runtime_error("Cannot open file" + std::string(fileName));
	}

	// Read and validate the tag. It should be 66, 77, 70, 2,
	// or 'BMF' and 2 where the number is the file version.
	char magicString[5];
	SDL_ReadIO(f, magicString, 4);
	magicString[4] = 0;
	if (strncmp(magicString, "BMF\003", 4) != 0) {
		throw std::runtime_error("Unrecognized file format");
	}

	std::vector<Glyph> glyphs;

	char blockType;
	while (SDL_ReadIO(f, &blockType, 1)) {
		int blockSize;
		SDL_ReadIO(f, &blockSize, 4);

		switch (blockType) {
		case 1: // info
		case 2: // common
		case 3: // pages
		case 5: // kerning pairs
			SDL_SeekIO(f, blockSize, SDL_IO_SEEK_CUR);
			break;
		case 4: // chars
			if (blockSize % sizeof(Glyph)) {
				throw std::runtime_error("Invalid block sie");
			}
			glyphs.resize(blockSize / sizeof(Glyph));
			SDL_ReadIO(f, glyphs.data(), blockSize);
			break;
		default:
			throw std::runtime_error("Unexpected block type");
		}
	}

	// Check pages
	for (const Glyph& g : glyphs) {
		if (g.page != 0) {
			throw std::runtime_error("Only one page supported");
		}
	}

	// Sort glyphs to speedup search of chars
	std::sort(std::begin(glyphs), std::end(glyphs));

	SDL_CloseIO(f);
	return glyphs;
}

} // namespace Wind
