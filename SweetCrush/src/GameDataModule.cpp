#include "GameDataModule.h"

#if defined (__ANDROID__)

#include <gameData/Levels.h>

GameDataModule::GameDataModule()
    : GetNumLevels(::GetNumLevels)
    , GetLevel(::GetLevel) {
}

#else

GameDataModule::GameDataModule()
    : GetNumLevels(nullptr)
    , GetLevel(nullptr) {
}

bool GameDataModule::Init(const char* dllFileName) {
	if (DLLError err = dll.Load(dllFileName); err != DLLError::ok) {
		return false;
	}
	return GetProcedures();
}

bool GameDataModule::Reload() {
	// FIXME Release current DLL on success only ?
	if (dll.Reload() == DLLError::ok) {
		return GetProcedures();
	}
	return false;
}

bool GameDataModule::GetProcedures() {
	if (DLLError err = dll.GetProcedure(&GetNumLevels, "GetNumLevels"); err != DLLError::ok) {
		return false;
	}
	if (DLLError err = dll.GetProcedure(&GetLevel, "GetLevel"); err != DLLError::ok) {
		return false;
	}
	return true;
}

#endif
