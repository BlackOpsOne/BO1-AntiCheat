#include "pch.h"

#include "dvar.h"

__declspec(selectany) auto Dvar_GetString = reinterpret_cast<const char* (*)(const char*)>(0x57ff80);
__declspec(selectany) auto Dvar_GetBool = reinterpret_cast<bool (*)(const char*)>(0x68b030);

extern "C" __declspec(dllexport) bool __cdecl GetDvarBool(const char* dvarName)
{
    bool value = Dvar_GetBool(dvarName);
    return value;
}

extern "C" __declspec(dllexport) const char* __cdecl GetDvarString(const char* dvarName)
{
    const char* value = Dvar_GetString(dvarName);

    if (value == nullptr)
    {
        value = "null";
    }

    return value;
}

void KeepAliveThread()
{
    while (true) Sleep(1000);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
            CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)KeepAliveThread, nullptr, 0, nullptr);
            break;
    }
    return TRUE;
}
