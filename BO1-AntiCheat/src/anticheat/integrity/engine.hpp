#include <Windows.h>

#include <TlHelp32.h>

#include <psapi.h>

#include <iostream>

#include <filesystem>

#include <map>

#include <vector>
#include <string>

namespace anticheat {
	namespace integrity {
		namespace engine {
			typedef const char* (__cdecl* Dvar_GetString_t)(const char* dvarName);

			struct RemoteData
			{
				Dvar_GetString_t Dvar_GetString;
				const char* dvarName;
				const char* result;
			};

			// checks specific engine functions to make sure they are not modified
			// this is done by scanning the function opcodes
			std::string ModifiedEngineFunctions();

			// checks all player states for god mode, demigod mode and no target
			std::string GetModifiedPlayerStates();

			// older checks for stealth patches
			// these will be kept around just in case
			bool IsStealthPatchInjected();
			bool IsSuspiciousModule(HMODULE hModule, HANDLE hProcess);
		}
	}
}