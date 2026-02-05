#pragma once

#include "FwdDecl.h"

#include <vector>

namespace Wind {

enum class FontId : uint32_t {
};

class FontManager final {
public:
	explicit FontManager(Graphics& graphics);
	~FontManager();

	//	FontId      AddFont(const char* fontName);
	FontPtr     AddFont(const char* fontName);
	bool        IsValid(FontId fontId) const;
	const Font& GetFont(FontId fontId) const;
	const Font* TryGetFont(FontId fontId) const;

private:
	Graphics&            mGraphics;
	std::vector<FontPtr> mFonts;
};

struct Glyph;
std::vector<Glyph> LoadGlyphs(const char* fontFile);

} // namespace Wind
