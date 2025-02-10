#include "pch.h"

#include "dvar.h"

__declspec(selectany) auto Dvar_GetString = reinterpret_cast<const char* (*)(const char*)>(0x57FF80);
__declspec(selectany) auto Dvar_GetBool = reinterpret_cast<bool (*)(const char*)>(0x68B030);
__declspec(selectany) auto Dvar_GetInt = reinterpret_cast<unsigned int (*)(const char*)>(0x636670);
__declspec(selectany) auto Dvar_GetDouble = reinterpret_cast<double (*)(const char*)>(0x561EB0);

extern "C" __declspec(dllexport) LPVOID __cdecl GetDvarDouble(const char* dvarName)
{
    double value = Dvar_GetDouble(dvarName);
    std::string value_str = std::to_string(value);

    size_t size = value_str.size() + 1;
    LPVOID remote_mem = VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    if (!remote_mem)
        return nullptr;

    memcpy(remote_mem, value_str.c_str(), size);
    return remote_mem;
}

extern "C" __declspec(dllexport) LPVOID __cdecl GetDvarInt(const char* dvarName)
{
    unsigned int value = Dvar_GetInt(dvarName);
    std::string value_str = std::to_string(value);

    size_t size = value_str.size() + 1;
    LPVOID remote_mem = VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    if (!remote_mem)
        return nullptr;

    memcpy(remote_mem, value_str.c_str(), size);
    return remote_mem;
}

extern "C" __declspec(dllexport) LPVOID __cdecl GetDvarBool(const char* dvarName)
{
    bool value = Dvar_GetBool(dvarName);
    std::string value_str = value ? "true" : "false";

    size_t size = value_str.size() + 1;
    LPVOID remote_mem = VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    if (!remote_mem)
        return nullptr;

    memcpy(remote_mem, value_str.c_str(), size);
    return remote_mem;
}


extern "C" __declspec(dllexport) LPVOID __cdecl GetDvarString(const char* dvarName)
{
    const char* value = Dvar_GetString(dvarName);

    if (!value)
    {
        value = "null";
    }

    size_t size = strlen(value) + 1;
    LPVOID remote_mem = VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    if (!remote_mem)
        return nullptr;

    memcpy(remote_mem, value, size);
    return remote_mem;
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
