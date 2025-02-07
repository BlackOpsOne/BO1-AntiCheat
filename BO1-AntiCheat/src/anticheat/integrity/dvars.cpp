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

namespace anticheat {
	namespace integrity {
		namespace dvars {
			bool CallGetDvarBool(const char* dvar_name)
			{
				HANDLE process = game::process::GetBlackOpsProcess();
				if (!process)
				{
					return -1;
				}

				// get the module of the helper
				HMODULE helper_module = utils::memory::GetRemoteModuleHandle(process, Constants::HELPER_NAME.c_str());
				if (!helper_module)
				{
					return -1;
				}

				// get GetDvarBool address
				FARPROC func_address = utils::memory::GetRemoteProcAddress(process, helper_module, "GetDvarBool");
				if (!func_address)
				{
					return -1;
				}

				// allocate memory for the dvar name
				size_t len = strlen(dvar_name) + 1;
				LPVOID dvar_name_alloc = VirtualAllocEx(process, nullptr, len, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
				if (!dvar_name_alloc)
				{
					return -1;
				}

				// write the dvar name to memory
				WriteProcessMemory(process, dvar_name_alloc, dvar_name, len, nullptr);

				// create thread to call GetDvarBool
				HANDLE remote_thread = CreateRemoteThread(process, nullptr, 0, (LPTHREAD_START_ROUTINE)func_address, dvar_name_alloc, 0, nullptr);
				if (!remote_thread)
				{
					VirtualFreeEx(process, dvar_name_alloc, 0, MEM_RELEASE);
					return -1;
				}

				// wait til its done
				WaitForSingleObject(remote_thread, INFINITE);

				// get return value
				DWORD return_value;
				GetExitCodeThread(remote_thread, &return_value);

				// clean up
				VirtualFreeEx(process, dvar_name_alloc, 0, MEM_RELEASE);
				CloseHandle(remote_thread);

				// cast the return value to a boolean
				int value = static_cast<int>(return_value) & 1;
				return value == 1;
			}

			int CallGetDvarInt(const char* dvar_name)
			{
				return -1;
			}

			const char* CallGetDvarString(const char* dvar_name)
			{
				HANDLE process = game::process::GetBlackOpsProcess();
				if (!process)
				{
					return nullptr;
				}

				// get the module of the helper
				HMODULE helper_module = utils::memory::GetRemoteModuleHandle(process, Constants::HELPER_NAME.c_str());
				if (!helper_module)
				{
					return nullptr;
				}

				// get GetDvarBool address
				FARPROC func_address = utils::memory::GetRemoteProcAddress(process, helper_module, "GetDvarString");
				if (!func_address)
				{
					return nullptr;
				}

				// allocate memory for the dvar name
				size_t len = strlen(dvar_name) + 1;
				LPVOID dvar_name_alloc = VirtualAllocEx(process, nullptr, len, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
				if (!dvar_name_alloc)
				{
					return nullptr;
				}

				// write the dvar name to memory
				WriteProcessMemory(process, dvar_name_alloc, dvar_name, len, nullptr);

				// create thread to call GetDvarBool
				HANDLE remote_thread = CreateRemoteThread(process, nullptr, 0, (LPTHREAD_START_ROUTINE)func_address, dvar_name_alloc, 0, nullptr);
				if (!remote_thread)
				{
					VirtualFreeEx(process, dvar_name_alloc, 0, MEM_RELEASE);
					return nullptr;
				}

				// wait til its done
				WaitForSingleObject(remote_thread, INFINITE);

				// get the return value pointer
				DWORD return_value;
				GetExitCodeThread(remote_thread, &return_value);

				// read the return value pointer
				char buffer[256] = { 0 };
				ReadProcessMemory(process, (LPCVOID)return_value, buffer, sizeof(buffer), nullptr);

				// clean up
				VirtualFreeEx(process, dvar_name_alloc, 0, MEM_RELEASE);
				CloseHandle(remote_thread);

				// return the value, note this is a local copy
				return _strdup(buffer);
			}
		}
	}
}