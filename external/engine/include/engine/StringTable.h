#pragma once

#include <cstddef>

namespace Wind {

using StringId = unsigned int;

void        SetStringTable(const char** strings, size_t numStrings);
const char* GetString(StringId str);

} // namespace Wind
