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