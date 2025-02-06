#include "hooks.hpp"

#include <iostream>
#include <vector>
#include <string>

#include <shlwapi.h>
#include <windows.h>
#include "detours/detours.h"

LPVOID original_LoadLibraryA = nullptr;
LPVOID original_LoadLibraryW = nullptr;

bool isSystemDLL(LPCWSTR moduleName)
{
    WCHAR fullPath[MAX_PATH] = { 0 };

    if (!SearchPathW(NULL, moduleName, NULL, MAX_PATH, fullPath, NULL))
    {
        return false;
    }

    WCHAR systemPath[MAX_PATH] = { 0 };
    if (!GetSystemDirectoryW(systemPath, MAX_PATH))
    {
        return false;
    }

    if (PathIsPrefixW(systemPath, fullPath) || wcsstr(fullPath, L"\\System32\\") != nullptr)
    {
        return true;
    }

    return false;
}

bool isModuleAllowedW(LPCWSTR moduleName)
{
    return isSystemDLL(moduleName);
}

bool isModuleAllowedA(LPCSTR moduleName)
{
    std::wstring wideModuleName = std::wstring(moduleName, moduleName + strlen(moduleName));
    return isSystemDLL(wideModuleName.c_str());
}

extern "C" {
    __declspec(dllexport) HMODULE WINAPI Hooked_LoadLibraryA(LPCSTR lpLibFileName)
    {
        if (lpLibFileName)
        {
            if (!isModuleAllowedA(lpLibFileName))
            {
                SetLastError(ERROR_ACCESS_DENIED);
                return NULL;
            }
        }
        return reinterpret_cast<HMODULE(WINAPI*)(LPCSTR)>(original_LoadLibraryA)(lpLibFileName);
    }

    __declspec(dllexport) HMODULE WINAPI Hooked_LoadLibraryW(LPCWSTR lpLibFileName)
    {
        if (lpLibFileName)
        {
            if (!isModuleAllowedW(lpLibFileName))
            {
                SetLastError(ERROR_ACCESS_DENIED);
                return NULL;
            }
        }
        return reinterpret_cast<HMODULE(WINAPI*)(LPCWSTR)>(original_LoadLibraryW)(lpLibFileName);
    }
}

namespace anticheat {
    namespace hooks {
        bool Initialize()
        {
            HMODULE kernel32 = GetModuleHandle(L"kernel32.dll");
            if (!kernel32)
            {
                return false;
            }

            original_LoadLibraryA = GetProcAddress(kernel32, "LoadLibraryA");
            original_LoadLibraryW = GetProcAddress(kernel32, "LoadLibraryW");

            if (!original_LoadLibraryA || !original_LoadLibraryW) {
                return false;
            }

            DetourTransactionBegin();
            DetourUpdateThread(GetCurrentThread());
            DetourAttach(&original_LoadLibraryA, Hooked_LoadLibraryA);
            DetourAttach(&original_LoadLibraryW, Hooked_LoadLibraryW);

            if (DetourTransactionCommit() == NO_ERROR) {
                return true;
            }

            return false;
        }

        void Cleanup() {
            DetourTransactionBegin();
            DetourUpdateThread(GetCurrentThread());
            DetourDetach(&original_LoadLibraryA, Hooked_LoadLibraryA);
            DetourDetach(&original_LoadLibraryW, Hooked_LoadLibraryW);
            DetourTransactionCommit();
        }
    }
}