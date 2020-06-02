#include "StringTable.h"
#include <cassert>

namespace Wind {

namespace {
	const char** strTable = nullptr;
	size_t strTableSize = 0;
}

void SetStringTable(const char** strings, size_t numStrings) {
	strTable = strings;
	strTableSize = numStrings;
}

const char* GetString(StringId str) {
	assert(strTable);
	assert(str < strTableSize);
	return strTable[str];
}

}
