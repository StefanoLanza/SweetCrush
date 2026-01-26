#pragma once

#include <inih/ini.h>

#include <algorithm>
#include <cstring>
#include <functional>

namespace Wind {

#define PARSE_INT(var, iniVarName, minv, maxv)          \
	if (! strcmp(name, iniVarName)) {                   \
		var = std::clamp(std::atoi(value), minv, maxv); \
	}

#define PARSE_FLOAT(var, iniVarName, minv, maxv)                          \
	do {                                                                  \
		if (! strcmp(name, iniVarName)) {                                 \
			var = std::clamp<float>((float)std::atof(value), minv, maxv); \
		}                                                                 \
	} while (0)

#define PARSE_BOOL(var, iniVarName)                 \
	if (! strcmp(name, iniVarName)) {               \
		var = strcmp(value, "true") ? false : true; \
	}

#define PARSE_STRING(var, iniVarName) \
	if (! strcmp(name, iniVarName)) { \
		var = value;                  \
	}

int ParseINIFile(const char* fileName, ini_handler handler, void* user);

using INIListener = std::function<void(const char* varName, const char* varValue)>;

class INIParser final {
public:
	void AddListener(const char* section, INIListener&& listener);
	int  ParseFile(const char* fileName) const;

private:
	static int Handler(void* user, const char* section, const char* name, const char* value);

	std::vector<std::pair<const char*, INIListener>> mListeners;
};

} // namespace Wind
