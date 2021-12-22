#include "Font.h"
#include "SdlSurface.h"
#include <SDL/include/SDL.h>
#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <string>

namespace Wind {

Font::Font(const char* name, const char* textureFile, const char* texturePath, std::vector<Glyph>&& glyphs)
    : mName(name)
    , mSurface(textureFile, texturePath)
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

const SdlSurface& Font::GetSurface() const {
	return mSurface;
}

const Glyph& Font::FindGlyph(char c) const {
	if (const Glyph* g = TryFindGlyph(c); g) {
		return *g;
	}
	return mDefaultGlyph;
}

int Font::CalculateStringWidth(const char* text) const {
	assert(text);
	int advance = 0;
	for (; *text; ++text) {
		const Glyph& g = FindGlyph(*text);
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
	SDL_RWops* f = SDL_RWFromFile(fileName, "rb");
	if (! f) {
		throw std::runtime_error("Cannot open file" + std::string(fileName));
	}

	// Read and validate the tag. It should be 66, 77, 70, 2,
	// or 'BMF' and 2 where the number is the file version.
	char magicString[4];
	SDL_RWread(f, magicString, 4, 1);
	if (strncmp(magicString, "BMF\003", 4) != 0) {
		throw std::runtime_error("Unrecognized file format");
	}

	std::vector<Glyph> glyphs;

	char blockType;
	while (SDL_RWread(f, &blockType, 1, 1)) {
		int blockSize;
		SDL_RWread(f, &blockSize, 4, 1);

		switch (blockType) {
		case 1: // info
		case 2: // common
		case 3: // pages
		case 5: // kerning pairs
			SDL_RWseek(f, blockSize, RW_SEEK_CUR);
			break;
		case 4: // chars
			if (blockSize % sizeof(Glyph)) {
				throw std::runtime_error("Invalid block sie");
			}
			glyphs.resize(blockSize / sizeof(Glyph));
			SDL_RWread(f, glyphs.data(), blockSize, 1);
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
	std::sort(std::begin(glyphs), std::end(glyphs));

	SDL_RWclose(f);
	return glyphs;
}

} // namespace Wind
