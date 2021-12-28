#pragma once

#include <cassert>
#include <cstdint>
#include <string>

struct FileTime {
	uint32_t lowDateTime;
	uint32_t highDateTime;
};

enum class DLLError
{
	copyFailed = 0,
	unchanged = 1,
	getProcAddressFailed = 2,
	loadLibraryFailed = 3,
	ok = 4
};

#if defined(_WIN32)
using DLLProc = void(__cdecl*)(void);
#elif defined(__linux__)
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
