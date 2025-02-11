#include "memory.hpp"

#include "../game/process.hpp"
#include "../constants.h"
#include "files.hpp"

namespace anticheat {
	namespace utils {
		namespace memory {
			int ReadInt(HANDLE processHandle, int address) {
				if (address == -1 || processHandle == nullptr) {
					return -1;
				}

				int buffer = 0;
				SIZE_T bytesRead = 0;
				bool success = ReadProcessMemory(processHandle, (LPCVOID)address, &buffer, sizeof(buffer), &bytesRead);

				if (!success || bytesRead != sizeof(buffer)) {
					return -1;
				}

				return buffer;
			}

			std::string ReadString(HANDLE processHandle, int address) {
				if (address == -1 || processHandle == nullptr) {
					return "";
				}

				char buffer = 0;
				std::string result;
				SIZE_T bytesRead = 0;

				// Read std::string byte-by-byte until null terminator
				while (true) {
					bool success = ReadProcessMemory(processHandle, (LPCVOID)address, &buffer, sizeof(buffer), &bytesRead);
					if (!success || bytesRead != sizeof(buffer) || buffer == 0) {
						break;
					}

					result.push_back(buffer);
					address++;
				}

				return result;
			}

			std::vector<BYTE> GetFunctionOpcodes(void* targetAddr, size_t opcodeSize) {
				HANDLE handle = game::process::GetBlackOpsHandle();
				std::vector<BYTE> buffer(opcodeSize);
				SIZE_T bytesRead;

				std::vector<BYTE> opcodes;

				if (!game::process::IsGameOpen()) {
					return opcodes;
				}

				try {
					if (ReadProcessMemory(handle, targetAddr, buffer.data(), buffer.size(), &bytesRead) && bytesRead == buffer.size()) {
						if (!game::process::IsGameOpen()) {
							return opcodes;
						}

						opcodes = std::move(buffer);
					}
				}
				catch (const std::exception&) {
					return opcodes;
				}

				return opcodes;
			}

			bool DoOpcodesMatch(const std::vector<BYTE>& actualOpcodes, const std::vector<BYTE>& expectedOpcodes) {
				// check size
				if (actualOpcodes.size() != expectedOpcodes.size()) {
					// they may try to close the game during this
					if (!game::process::IsGameOpen())
					{
						return true;
					}
					return false;
				}

				// compare the bytes
				for (size_t i = 0; i < actualOpcodes.size(); ++i) {
					if (actualOpcodes[i] != expectedOpcodes[i]) {
						// they may try to close the game during this
						if (!game::process::IsGameOpen())
						{
							return true;
						}
						return false; // did not match
					}
				}

				return true; // all matched
			}

			bool InjectDLL(HANDLE process, const char* dll_path)
			{
				LPVOID pDllPath = VirtualAllocEx(process, NULL, strlen(dll_path) + 1, MEM_COMMIT, PAGE_READWRITE);
				if (!pDllPath)
				{
					return false;
				}

				if (!WriteProcessMemory(process, pDllPath, (LPVOID)dll_path, strlen(dll_path) + 1, NULL))
				{
					VirtualFreeEx(process, pDllPath, 0, MEM_RELEASE);
					return false;
				}

				HANDLE hThread = CreateRemoteThread(process, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, pDllPath, 0, NULL);
				if (!hThread)
				{
					VirtualFreeEx(process, pDllPath, 0, MEM_RELEASE);
					return false;
				}

				WaitForSingleObject(hThread, INFINITE);

				VirtualFreeEx(process, pDllPath, 0, MEM_RELEASE);
				CloseHandle(hThread);

				return true;
			}

			FARPROC GetRemoteProcAddress(HANDLE process, HMODULE module, const char* function_name)
			{
				if (!module)
					return nullptr;

				// Read the IMAGE_DOS_HEADER
				IMAGE_DOS_HEADER dosHeader;
				if (!ReadProcessMemory(process, (LPCVOID)module, &dosHeader, sizeof(IMAGE_DOS_HEADER), nullptr) ||
					dosHeader.e_magic != IMAGE_DOS_SIGNATURE)
				{
					return nullptr;
				}

				// Read the IMAGE_NT_HEADERS
				IMAGE_NT_HEADERS ntHeaders;
				if (!ReadProcessMemory(process, (LPCVOID)((BYTE*)module + dosHeader.e_lfanew), &ntHeaders, sizeof(IMAGE_NT_HEADERS), nullptr) ||
					ntHeaders.Signature != IMAGE_NT_SIGNATURE)
				{
					return nullptr;
				}

				// Read the IMAGE_EXPORT_DIRECTORY
				IMAGE_DATA_DIRECTORY exportDataDir = ntHeaders.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
				IMAGE_EXPORT_DIRECTORY exportDirectory;
				if (!ReadProcessMemory(process, (LPCVOID)((BYTE*)module + exportDataDir.VirtualAddress), &exportDirectory, sizeof(IMAGE_EXPORT_DIRECTORY), nullptr))
				{
					return nullptr;
				}

				// Read function names and addresses
				DWORD* namePointers = new DWORD[exportDirectory.NumberOfNames];
				WORD* ordinalTable = new WORD[exportDirectory.NumberOfNames];
				DWORD* functionPointers = new DWORD[exportDirectory.NumberOfFunctions];

				ReadProcessMemory(process, (LPCVOID)((BYTE*)module + exportDirectory.AddressOfNames), namePointers, exportDirectory.NumberOfNames * sizeof(DWORD), nullptr);
				ReadProcessMemory(process, (LPCVOID)((BYTE*)module + exportDirectory.AddressOfNameOrdinals), ordinalTable, exportDirectory.NumberOfNames * sizeof(WORD), nullptr);
				ReadProcessMemory(process, (LPCVOID)((BYTE*)module + exportDirectory.AddressOfFunctions), functionPointers, exportDirectory.NumberOfFunctions * sizeof(DWORD), nullptr);

				// Iterate through function names to find the matching one
				FARPROC remoteFunctionAddress = nullptr;
				for (DWORD i = 0; i < exportDirectory.NumberOfNames; i++)
				{
					char functionNameBuffer[256] = { 0 };
					ReadProcessMemory(process, (LPCVOID)((BYTE*)module + namePointers[i]), functionNameBuffer, sizeof(functionNameBuffer), nullptr);

					if (_stricmp(function_name, functionNameBuffer) == 0)
					{
						WORD functionOrdinal = ordinalTable[i];
						DWORD functionRVA = functionPointers[functionOrdinal];
						remoteFunctionAddress = (FARPROC)((BYTE*)module + functionRVA);
						break;
					}
				}

				// Cleanup
				delete[] namePointers;
				delete[] ordinalTable;
				delete[] functionPointers;

				return remoteFunctionAddress;
			}

			HMODULE GetRemoteHelperModule(HANDLE handle)
			{
				HMODULE hMods[1024];
				DWORD cbNeeded;

				if (EnumProcessModules(handle, hMods, sizeof(hMods), &cbNeeded))
				{
					for (size_t i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
					{
						char modName[MAX_PATH];
						if (GetModuleBaseNameA(handle, hMods[i], modName, sizeof(modName) / sizeof(char)))
						{
							char moduleFilePath[MAX_PATH];
							if (GetModuleFileNameExA(handle, hMods[i], moduleFilePath, sizeof(moduleFilePath)))
							{
								std::string module_hash = utils::files::GetMD5(moduleFilePath);

								if (_stricmp(modName, Constants::INTERNALS_NAME.c_str()) == 0
									&& (module_hash == Constants::INTERNALS_HASH))
								{
									return hMods[i];
								}
							}
						}
					}
				}

				return nullptr;
			}

			std::vector<ModuleInfo> GetProcessModules(HANDLE handle)
			{
				std::vector<ModuleInfo> modules;
				HMODULE hMods[1024];
				DWORD cbNeeded;

				if (EnumProcessModules(handle, hMods, sizeof(hMods), &cbNeeded))
				{
					for (size_t i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
					{
						char modName[MAX_PATH];
						char moduleFilePath[MAX_PATH];

						if (GetModuleBaseNameA(handle, hMods[i], modName, sizeof(modName)) &&
							GetModuleFileNameExA(handle, hMods[i], moduleFilePath, sizeof(moduleFilePath)))
						{
							ModuleInfo modInfo;
							modInfo.name = modName;
							modInfo.md5 = utils::files::GetMD5(moduleFilePath).c_str();
							modInfo.is_signed = utils::files::IsDigitallySigned(moduleFilePath);
							modules.push_back(modInfo);
						}
					}
				}

				return modules;
			}
		} // memory
	} // utils
} // anticheat