#ifndef UTILS_H
#define UTILS_H

#include<Windows.h>
#include<TlHelp32.h>

DWORD GetProcId(const wchar_t* procName);
uintptr_t GetModuleBaseAddress(DWORD procId, wchar_t* modName);

#endif