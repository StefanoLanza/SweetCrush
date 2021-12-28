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
	mModule = nullptr;
	mFileName = fileName;
	mWriteTime = GetLastWriteTime(fileName);

	const char* toLoad = fileName;
	char        tmpFileName[MAX_PATH];
	if (mVersion % 2 == 0) {
		// Copy DLL to a temporary file so that we can recompile it while the process is running
		sprintf_s(tmpFileName, "%s_temp", fileName);
		const BOOL copyRes = CopyFileA(fileName, tmpFileName, false); // false: overwrite
		if (copyRes == FALSE) {
			return DLLError::copyFailed;
		}
		toLoad = tmpFileName;
	}

	const HMODULE module = LoadLibraryA(toLoad);
	if (! module) {
		return DLLError::loadLibraryFailed;
	}
	Free();
	mModule = module;
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
		DLL      tmp_dll { *this };
		DLLError err = tmp_dll.Load(mFileName.c_str());
		if (err == DLLError::ok) {
			// Replace DLL on success
			Free();
			*this = tmp_dll;
		}
		return err;
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
