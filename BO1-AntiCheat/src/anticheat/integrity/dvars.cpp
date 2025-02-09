#include "dvars.hpp"

#include <windows.h>
#include <iostream>
#include <filesystem>

#include "../../utils/files.hpp"
#include "../../utils/memory.hpp"

#include "../../game/process.hpp"
#include "../../game/game.hpp"

#include "checksums.h"
#include "../../constants.h"

#include "../../utils/strings.hpp"

#include <vector>

std::vector<DvarIntEntry> dvar_int_queue;

namespace anticheat {
	namespace integrity {
		namespace dvars {
            typedef const char* (__cdecl* Dvar_GetString_t)(const char* dvarName);
            typedef unsigned int(__cdecl* Dvar_GetInt_t)(const char* dvarName);
            typedef bool(__cdecl* Dvar_GetBool_t)(const char* dvarName);

            static Dvar_GetString_t Dvar_GetString = reinterpret_cast<Dvar_GetString_t>(0x0057FF80);
            static Dvar_GetInt_t Dvar_GetInt = reinterpret_cast<Dvar_GetInt_t>(0x636670);
            static Dvar_GetBool_t Dvar_GetBool = reinterpret_cast<Dvar_GetBool_t>(0x68b030);

            void InitDvarQueue()
            {
                dvar_int_queue = {
                    { "cl_noprint", 0 }
                };
            }

			std::string GetModifiedDvars()
			{
				if (!game::process::IsGameOpen())
				{
					return "";
				}
                vector<string> modified_dvars;

                // check the integer based dvars
                for (DvarIntEntry int_entry : dvar_int_queue)
                {
                    if (!game::process::IsGameOpen())
                    {
                        return "";
                    }

                    int game_value = GetDvarInt(int_entry.dvar_name);

                    if (game_value != int_entry.expected_value)
                    {
                        std::string name_str(int_entry.dvar_name);
                        modified_dvars.push_back(name_str);
                    }
                }

                return utils::strings::FormatVector(modified_dvars);
			}

            const char* GetDvarString(const char* dvar_name)
            {
                HANDLE handle = game::process::GetBlackOpsProcess();
                if (!handle)
                {
                    return nullptr;
                }

                // allocate memory for the dvar name
                size_t len = strlen(dvar_name) + 1;
                LPVOID remote_dvar_name = VirtualAllocEx(handle, nullptr, len, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
                if (!remote_dvar_name)
                {
                    return nullptr;
                }

                // write the dvar name to the game
                WriteProcessMemory(handle, remote_dvar_name, dvar_name, len, nullptr);

                // allocate memory for the return value
                LPVOID remote_return_value = VirtualAllocEx(handle, nullptr, sizeof(const char*), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
                if (!remote_return_value)
                {
                    VirtualFreeEx(handle, remote_dvar_name, 0, MEM_RELEASE);
                    return nullptr;
                }

                // create a remote thread to call the function
                HANDLE remote_thread = CreateRemoteThread(handle, nullptr, 0, (LPTHREAD_START_ROUTINE)Dvar_GetString, remote_dvar_name, 0, nullptr);
                if (!remote_thread)
                {
                    VirtualFreeEx(handle, remote_dvar_name, 0, MEM_RELEASE);
                    VirtualFreeEx(handle, remote_return_value, 0, MEM_RELEASE);
                    return nullptr;
                }

                // wait for the function to execute
                WaitForSingleObject(remote_thread, INFINITE);

                // get the return value
                DWORD return_value;
                GetExitCodeThread(remote_thread, &return_value);

                // read the string from memory
                char buffer[256] = { 0 };
                ReadProcessMemory(handle, (LPCVOID)return_value, buffer, sizeof(buffer), nullptr);

                // clean up
                VirtualFreeEx(handle, remote_dvar_name, 0, MEM_RELEASE);
                VirtualFreeEx(handle, remote_return_value, 0, MEM_RELEASE);
                CloseHandle(remote_thread);

                // return the value (this is a local copy)
                return _strdup(buffer);
            }

            unsigned int GetDvarInt(const char* dvar_name)
            {
                HANDLE process = game::process::GetBlackOpsProcess();
                if (!process)
                {
                    return -1;
                }

                // allocate memory for the dvar name
                size_t len = strlen(dvar_name) + 1;
                LPVOID remoteDvarName = VirtualAllocEx(process, nullptr, len, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
                if (!remoteDvarName)
                {
                    return -1;
                }

                // write the dvar name to the game
                WriteProcessMemory(process, remoteDvarName, dvar_name, len, nullptr);

                // allocate memory for the return value
                LPVOID remote_return_value = VirtualAllocEx(process, nullptr, sizeof(unsigned int), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
                if (!remote_return_value)
                {
                    VirtualFreeEx(process, remoteDvarName, 0, MEM_RELEASE);
                    return -1;
                }

                // create a remote thread to call the function
                HANDLE remote_thread = CreateRemoteThread(process, nullptr, 0, (LPTHREAD_START_ROUTINE)Dvar_GetInt, remoteDvarName, 0, nullptr);
                if (!remote_thread)
                {
                    VirtualFreeEx(process, remoteDvarName, 0, MEM_RELEASE);
                    VirtualFreeEx(process, remote_return_value, 0, MEM_RELEASE);
                    return -1;
                }

                // wait for the function to execute
                WaitForSingleObject(remote_thread, INFINITE);

                // get the return value
                DWORD return_value;
                GetExitCodeThread(remote_thread, &return_value);

                // clean up
                VirtualFreeEx(process, remoteDvarName, 0, MEM_RELEASE);
                VirtualFreeEx(process, remote_return_value, 0, MEM_RELEASE);
                CloseHandle(remote_thread);

                // return the value (dvar ints are always an unsigned int)
                return static_cast<unsigned int>(return_value);
            }

            bool GetDvarBool(const char* dvar_name)
            {
                HANDLE handle = game::process::GetBlackOpsProcess();
                if (!handle)
                {
                    return false;
                }

                // allocate memory for the dvar name
                size_t len = strlen(dvar_name) + 1;
                LPVOID remote_dvar_name = VirtualAllocEx(handle, nullptr, len, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
                if (!remote_dvar_name)
                {
                    return false;
                }

                // write the dvar name to the game
                WriteProcessMemory(handle, remote_dvar_name, dvar_name, len, nullptr);

                // allocate memory for the return value
                LPVOID remote_return_value = VirtualAllocEx(handle, nullptr, sizeof(bool), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
                if (!remote_return_value)
                {
                    VirtualFreeEx(handle, remote_dvar_name, 0, MEM_RELEASE);
                    return false;
                }

                // create a remote thread to call the function
                HANDLE remote_thread = CreateRemoteThread(handle, nullptr, 0, (LPTHREAD_START_ROUTINE)Dvar_GetBool, remote_dvar_name, 0, nullptr);
                if (!remote_thread)
                {
                    VirtualFreeEx(handle, remote_dvar_name, 0, MEM_RELEASE);
                    VirtualFreeEx(handle, remote_return_value, 0, MEM_RELEASE);
                    return false;
                }

                // wait for the function to execute
                WaitForSingleObject(remote_thread, INFINITE);

                // get the return value
                DWORD return_value;
                GetExitCodeThread(remote_thread, &return_value);

                // clean up
                VirtualFreeEx(handle, remote_dvar_name, 0, MEM_RELEASE);
                VirtualFreeEx(handle, remote_return_value, 0, MEM_RELEASE);
                CloseHandle(remote_thread);

                // return the value
                return static_cast<bool>(return_value);
            }
		}
	}
}