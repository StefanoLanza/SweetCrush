#include "GameDataModule.h"
#include <SDL/include/SDL_log.h>

#if defined(__ANDROID__)

#include "../gameData/Levels.h"

GameDataModule::GameDataModule()
    : GetNumLevels(::GetNumLevels)
    , GetLevel(::GetLevel) {
}

bool GameDataModule::Init([[maybe_unused]] const char* dllFileName) {
	return true;
}

bool GameDataModule::Reload() {
	return true;
}

bool GameDataModule::IsValid() const {
	return true;
}

#else

GameDataModule::GameDataModule()
    : GetNumLevels(nullptr)
    , GetLevel(nullptr)
    , mValid(false) {
}

bool GameDataModule::Init(const char* dllFileName) {
	assert(dllFileName);
	if (DLLError err = mDLL.Load(dllFileName); err != DLLError::ok) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to open DLL %s", dllFileName);
		mValid = false;
		return false;
	}
	mValid = GetProcedures();
	return mValid;
}

bool GameDataModule::Reload() {
	if (mDLL.Reload() == DLLError::ok) {
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "DLL %s reloaded", mDLL.GetFileName());
		mValid = GetProcedures();
		return mValid;
	}
	return false;
}

bool GameDataModule::GetProcedures() {
	if (DLLError err = mDLL.GetProcedure(&GetNumLevels, "GetNumLevels"); err != DLLError::ok) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to get address  of procedure %s", "GetNumLevels");
		return false;
	}
	if (DLLError err = mDLL.GetProcedure(&GetLevel, "GetLevel"); err != DLLError::ok) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to get address  of procedure %s", "GetLevel");
		return false;
	}
	return true;
}

bool GameDataModule::IsValid() const {
	return mValid;
}

#endif
