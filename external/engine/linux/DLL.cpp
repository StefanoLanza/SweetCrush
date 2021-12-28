#include "../DLL.h"

#include <cassert>
#include <dlfcn.h>

const char* GetErrorMessage(DLLError error)
{
	static const char* str[] =
	{
		"unchanged",
		"getProcAddressFailed",
		"loadLibraryFailed",
		"ok"
	};
	return str[(uint)error];
}


DLLError LoadDLL(DLL& dll, const char* fileName)
{
	dll.module = nullptr;
	dll.fileName = fileName;
	//dll.writeTime = 0; //GetLastWriteTime(fileName);

	// Copy DLL to a temporary file so that we can recompile it while the process is running
	//const char* toLoad = fileName;
	//char tmpFileName[MAX_PATH];

	void* const module = dlopen(fileName, RTLD_LOCAL | RTLD_LAZY); //RTLD_NOW);
	if (! module)
	{
		return DLLError::loadLibraryFailed;
	}
	dll.module = module;
	dll.version++;
	return DLLError::ok;
}


void FreeDLL(DLL& dll)
{
	if (dll.module)
	{
		dlclose(dll.module);
		dll.module = nullptr;
		dll.writeTime = {};
	}
}


DLLError ReloadDLL(DLL& dll)
{
	DLLError err = DLLError::unchanged;
    return err;
}


DLLProc GetDLLProcedure(const DLL& dll, const char* procName)
{
	assert(procName);
	if (! dll.module) {
        return nullptr; 
    }
	DLLProc procAddr = static_cast<DLLProc>(dlsym(dll.module, procName));
	return procAddr;
}


bool IsValid(const DLL& dll)
{
	return dll.module != nullptr;
}
