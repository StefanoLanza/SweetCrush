#pragma once

#include <cassert>
#include <cstdint>
#include <string>

enum class DLLError
{
	ok = 0,
	copyFailed = 1,
	unchanged = 2,
	getProcAddressFailed = 3,
	loadLibraryFailed = 4,
	getFileTimeStampFailed = 5,
};

#if defined(_WIN32)

struct FileTime {
	uint32_t lowDateTime;
	uint32_t highDateTime;
};
using DLLProc = void(__cdecl*)(void);

#elif defined(__linux__)

#include <ctime>

struct FileTime {
	time_t sec;
};
using DLLProc = void*;
#endif

class DLL {
public:
	DLL();

	DLLError Load(const char* fileName);
	void     Free();
	DLLError Reload();
	bool     IsValid() const;
	DLLProc  GetProcedure(const char* procedureName) const;
	const char* GetFileName() const;

	template <typename Proc>
	DLLError GetProcedure(Proc* outProc, const char* procName) const;

private:
	void*        mModule;
	std::string  mFileName;
	FileTime     mWriteTime;
	unsigned int mVersion;
};

const char* GetErrorMessage(DLLError error);

template <typename Proc>
DLLError DLL::GetProcedure(Proc* outProc, const char* procedureName) const {
	assert(outProc);
	DLLProc proc = GetProcedure(procedureName);
	*outProc = reinterpret_cast<Proc>(proc);
	return proc == nullptr ? DLLError::getProcAddressFailed : DLLError::ok;
}
