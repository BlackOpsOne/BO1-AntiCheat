#include "injections.hpp"

#include <iostream>
#include <vector>
#include <string>

#include <shlwapi.h>
#include <windows.h>
#include "detours/detours.h"

#include "../utils/files.hpp"
#include "../utils/strings.hpp"
#include "../constants.h"

LPVOID original_LoadLibraryA = nullptr;
LPVOID original_LoadLibraryW = nullptr;

extern "C" {
    __declspec(dllexport) HMODULE WINAPI Hooked_LoadLibraryA(LPCSTR lpLibFileName)
    {
        if (lpLibFileName)
        {
            if (!anticheat::integrity::injections::IsModuleAllowedA(lpLibFileName))
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
            if (!anticheat::integrity::injections::IsModuleAllowedW(lpLibFileName))
            {
                SetLastError(ERROR_ACCESS_DENIED);
                return NULL;
            }
        }
        return reinterpret_cast<HMODULE(WINAPI*)(LPCWSTR)>(original_LoadLibraryW)(lpLibFileName);
    }
}

namespace anticheat {
    namespace integrity {
        namespace injections {
            bool IsSystemDLL(LPCWSTR module_name)
            {
                WCHAR full_path[MAX_PATH] = { 0 };

                if (!SearchPathW(NULL, module_name, NULL, MAX_PATH, full_path, NULL))
                {
                    return false;
                }

                WCHAR system_path[MAX_PATH] = { 0 };
                if (!GetSystemDirectoryW(system_path, MAX_PATH))
                {
                    return false;
                }

                std::string dllPath = utils::strings::WStringToString(full_path);

                bool isSystemPath = PathIsPrefixW(system_path, full_path) 
                    || wcsstr(full_path, L"\\System32\\") != nullptr;

                bool isSigned = utils::files::IsDigitallySigned(dllPath);
                bool is_t5_internals = (utils::files::GetMD5(dllPath) == Constants::INTERNALS_HASH);

                return (isSystemPath && isSigned) || is_t5_internals;
            }

            bool IsModuleAllowedW(LPCWSTR module_name)
            {
                return IsSystemDLL(module_name);
            }

            bool IsModuleAllowedA(LPCSTR module_name)
            {
                std::wstring wide_name = std::wstring(module_name, module_name + strlen(module_name));
                return IsSystemDLL(wide_name.c_str());
            }

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
        } // injections
    } // integrity
} // anticheat