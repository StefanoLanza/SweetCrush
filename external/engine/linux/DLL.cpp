#include "../DLL.h"

#include <cassert>
#include <dlfcn.h>
#include <sys/stat.h>

namespace {

FileTime GetLastWriteTime(const char* path) {
	FileTime time = {};
	struct stat s;
	if (lstat(path, &s) == 0) {
		time.sec = s.st_mtim.tv_sec;
	}
	return time;
}

}

const char* GetErrorMessage(DLLError error) {
	static const char* str[] = {
		"unchanged",
		"getProcAddressFailed",
		"loadLibraryFailed",
		"ok"
	};
	return str[(int)error];
}

DLL::DLL()
	: mModule { nullptr }
	, mWriteTime {}
	, mVersion { 0 } {
}

DLLError DLL::Load(const char* fileName) {
	assert(fileName);
	Free();
	void* const module = dlopen(fileName, RTLD_LOCAL | RTLD_LAZY);
	if (! module) {
		return DLLError::loadLibraryFailed;
	}
	mFileName = fileName;
	mWriteTime = GetLastWriteTime(fileName);
	mModule = module;
	mVersion++;
	return DLLError::ok;
}


void DLL::Free() {
	if (mModule) {
		dlclose(mModule);
		mModule = nullptr;
		mWriteTime = {};
	}
}


DLLError DLL::Reload() {
	const FileTime newTime = GetLastWriteTime(mFileName.c_str());
	if (newTime.sec == 0) {
		return DLLError::getFileTimeStampFailed;
	}
	if (newTime.sec != mWriteTime.sec) {
		return Load(mFileName.c_str());
	}
	return DLLError::unchanged;
}

DLLProc DLL::GetProcedure(const char* procedureName) const {
	assert(procedureName);
	if (! mModule) {
		return nullptr;
	}
	return reinterpret_cast<DLLProc>(dlsym(mModule, procedureName));
}

bool DLL::IsValid() const {
	return mModule != nullptr;
}

const char* DLL::GetFileName() const {
	return mFileName.c_str();
}
