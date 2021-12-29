#pragma once

struct Level;

#if defined (__ANDROID__)

class GameDataModule {
public:
	using GetNumLevelsPtr = int (*)();
	using GetLevelPtr = const Level* (*)(int levelIndex);

	GameDataModule();

	bool Init(const char* dllFileName);
	bool Reload();
	bool IsValid() const;

	GetNumLevelsPtr GetNumLevels;
	GetLevelPtr     GetLevel;
};

#else

#include <engine/DLL.h>

class GameDataModule {
public:
	using GetNumLevelsPtr = int (*)();
	using GetLevelPtr = const Level* (*)(int levelIndex);

	GameDataModule();

	bool Init(const char* dllFileName);
	bool Reload();
	bool IsValid() const;

	GetNumLevelsPtr GetNumLevels;
	GetLevelPtr     GetLevel;

private:
	bool GetProcedures();

private:
	DLL mDLL;
	bool mValid;
};

#endif
