#include "../DLL.h"

#include <cassert>
#include <dlfcn.h>
#include <sys/stat.h>

namespace {

FileTime GetLastWriteTime(const char* path) {
	FileTime                  time = {};
	struct stat s;
  if ( lstat(path, &s) == 0 ) {
    time = s.tv_sec;
	}
	return time;
}

const char* GetErrorMessage(DLLError error)
{
	static const char* str[] =
	{
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

DLLError DLL::Load(const char* fileName)
{
	assert(fileName);

	mModule = nullptr;
	mFileName = fileName;
	mWriteTime = GetLastWriteTime(fileName);

	// Copy DLL to a temporary file so that we can recompile it while the process is running
	//const char* toLoad = fileName;
	//char tmpFileName[MAX_PATH];

	void* const module = dlopen(fileName, RTLD_LOCAL | RTLD_LAZY);
	if (! module)
	{
		return DLLError::loadLibraryFailed;
	}
	Free();
	mModule = module;
	mVersion++;
	return DLLError::ok;
}


void DLL::FreeDLL()
{
	if (mModule)
	{
		dlclose(mModule);
		mModule = nullptr;
		mWriteTime = {};
	}
}


DLLError DLL::Reload()
{
 	const FileTime newTime = GetLastWriteTime(mFileName.c_str());
	if (newTime.sec == 0) {
		return DLLError::getFileTimeStampFailed;
	}

	if (newTime.sec != mWriteTime.sec) {
		DLL      tmp_dll { *this };
		DLLError err = tmp_dll.Load(mFileName.c_str());
		if (err == DLLError::ok) {
			// Replace DLL on success
			Free();
			*this = tmp_dll;
		}
		return err;
	}
   return err;
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
