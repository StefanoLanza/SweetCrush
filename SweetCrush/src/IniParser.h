#pragma once

#include <inih/ini.h>

#include <algorithm>
#include <cstring>

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
