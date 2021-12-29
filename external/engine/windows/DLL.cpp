#include "../DLL.h"
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <cassert>
#include <cstdio>
#include <windows.h>

namespace {

FileTime GetLastWriteTime(const char* path) {
	FileTime                  time = {};
	WIN32_FILE_ATTRIBUTE_DATA data;
	if (GetFileAttributesExA(path, GetFileExInfoStandard, &data)) {
		time = { data.ftLastWriteTime.dwLowDateTime, data.ftLastWriteTime.dwHighDateTime };
	}
	return time;
}

} // namespace

const char* GetErrorMessage(DLLError error) {
	static const char* str[] = {
		"ok", "unchanged", "getProcAddressFailed", "loadLibraryFailed", "getFileTimeStampFailed",
	};
	return str[(int)error];
}

DLL::DLL()
    : mModule { nullptr }
    , mWriteTime {}
    , mVersion { 0 } {
}

DLLError DLL::Load(const char* fileName) {
	Free();

	char tmpFileName[MAX_PATH];
	// Copy DLL to a temporary file so that we can recompile it while the process is running
	sprintf_s(tmpFileName, "%s_temp", fileName);
	const BOOL copyRes = CopyFileA(fileName, tmpFileName, false); // false: overwrite
	if (copyRes == FALSE) {
		return DLLError::copyFailed;
	}

	const HMODULE module = LoadLibraryA(tmpFileName);
	if (module == NULL) {
		return DLLError::loadLibraryFailed;
	}

	mModule = module;
	mFileName = fileName;
	mWriteTime = GetLastWriteTime(fileName);
	mVersion++;
	return DLLError::ok;
}

void DLL::Free() {
	if (mModule) {
		FreeLibrary(static_cast<HMODULE>(mModule));
		mModule = nullptr;
		mWriteTime = {};
	}
}

DLLError DLL::Reload() {
	const FileTime newTime = GetLastWriteTime(mFileName.c_str());
	if (newTime.highDateTime == 0 && newTime.lowDateTime == 0) {
		return DLLError::getFileTimeStampFailed;
	}
	if (newTime.lowDateTime != mWriteTime.lowDateTime || newTime.highDateTime != mWriteTime.highDateTime) {
		return Load(mFileName.c_str());
	}
	return DLLError::unchanged;
}

DLLProc DLL::GetProcedure(const char* procedureName) const {
	assert(procedureName);
	if (! mModule) {
		return nullptr;
	}
	return reinterpret_cast<DLLProc>(GetProcAddress(static_cast<HMODULE>(mModule), procedureName));
}

bool DLL::IsValid() const {
	return mModule != nullptr;
}

const char* DLL::GetFileName() const {
	return mFileName.c_str();
}
