#include "FontManager.h"
#include "Font.h"
#include "Graphics.h"
#include "Config.h"

#include <SDL3/SDL.h>
#include <stdexcept>
#include <cassert>

namespace Wind {

FontManager::FontManager(Graphics& graphics)
    : mGraphics { graphics } {
}

FontManager::~FontManager() = default;

FontPtr FontManager::AddFont(const char* fontName) {
	try {
		for (const FontPtr& font : mFonts) {
			if (font->GetName() == fontName) {
				return font;
			}
		}

		char texturePath[260];
		snprintf(texturePath, sizeof(texturePath), "%s%s_0.png", FONTS_FOLDER, fontName);

		char glyphPath[260];
		snprintf(glyphPath, sizeof(glyphPath), "%s%s%s.fnt", ASSETS_FOLDER, FONTS_FOLDER, fontName);

		TexturePtr texture = mGraphics.LoadTexture(texturePath);

		mFonts.emplace_back(std::make_unique<Font>(fontName, texture, LoadGlyphs(glyphPath)));
		return mFonts.back();
	}
	catch (const std::exception& e) {
		SDL_LogError(0, "%s", e.what());
		return nullptr;
	}
}

bool FontManager::IsValid(FontId fontId) const {
	return false;
}

const Font& FontManager::GetFont(FontId fontId) const {
	// TODO: insert return statement here
	return *mFonts[0];
}

const Font* FontManager::TryGetFont(FontId fontId) const {
	return nullptr;
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
